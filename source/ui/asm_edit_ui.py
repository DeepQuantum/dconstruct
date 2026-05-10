from __future__ import annotations

import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

from PyQt6.QtCore import QSettings, Qt, pyqtSignal
from PyQt6.QtGui import QAction, QColor, QFont, QPalette, QTextCursor, QTextDocument, QTextFormat
from PyQt6.QtWidgets import (
    QApplication,
    QComboBox,
    QDialog,
    QFileDialog,
    QFormLayout,
    QGridLayout,
    QGroupBox,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QListWidget,
    QListWidgetItem,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QPlainTextEdit,
    QProgressDialog,
    QStatusBar,
    QTabWidget,
    QTextEdit,
    QVBoxLayout,
    QWidget,
    QStyleFactory,
)


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_GAMES = ("t2r", "t1x", "uc4")
HEADER_FILE_RE = re.compile(r"^#\s+listing for file:\s+(?P<path>.+)$")
HEADER_GAME_RE = re.compile(r"^#\s+game type:\s+(?P<game>\w+)$")
ROOT_CONTAINER_RE = re.compile(r"^(?P<indent>\s*)(?P<label>.+?)\s*=\s*(?P<type>.+?)\s+\[(?P<address>0x[0-9A-Fa-f]+)\](?P<suffix>.*)\{$")
MEMBER_CONTAINER_RE = re.compile(r"^(?P<indent>\s*)\[(?P<member>\d+)\]\s+(?P<type>.+?)\s+\[(?P<address>0x[0-9A-Fa-f]+)\](?P<suffix>.*)\{$")
VALUE_LINE_RE = re.compile(r"^(?P<indent>\s*)\[(?P<member>\d+)\]\s+(?P<type>[^:\[]+):\s*(?P<value>.*)$")
HEX_RE = re.compile(r"^0x[0-9A-Fa-f]+$")
SID_HASH_RE = re.compile(r"^#[0-9A-Fa-f]{1,16}$")
INT_RE = re.compile(r"^-?\d+$")
FLOAT_RE = re.compile(r"^-?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?$")


@dataclass(slots=True)
class ContainerContext:
    address: str
    depth: int
    label: str


@dataclass(slots=True)
class EditableField:
    line_number: int
    member_index: int
    parent_address: str
    value_type: str
    old_value: str
    line_text: str
    display_name: str
    raw_type: str

    @property
    def edit_expression_prefix(self) -> str:
        return f"{self.parent_address}[{self.member_index}]="


@dataclass(slots=True)
class SearchHit:
    tab_index: int
    title: str
    line_number: int
    line_text: str


@dataclass(slots=True)
class AsmDocument:
    display_path: Path
    source_bin_path: Path | None
    asm_text: str
    lines: list[str]
    editable_fields: dict[int, EditableField]
    game_type: str


def detect_default_dconstruct() -> Path | None:
    candidates = [
        ROOT / "build" / "Release" / "dconstruct.exe",
        ROOT / "build" / "Debug" / "dconstruct.exe",
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate

    for command_name in ("dconstruct.exe", "dconstruct"):
        resolved = shutil.which(command_name)
        if resolved:
            return Path(resolved).resolve()
    return None


def sidbase_next_to_dconstruct(dconstruct_path: Path) -> Path:
    return dconstruct_path.parent / "sidbase.bin"


def resolve_listing_path(raw_path: str) -> Path:
    candidate = Path(raw_path.strip())
    if candidate.is_absolute():
        return candidate
    return (ROOT / candidate).resolve()


def normalize_float_string(raw_value: str) -> str:
    try:
        value = float(raw_value)
    except ValueError as exc:
        raise ValueError("Expected a float value.") from exc
    if "." not in raw_value and "e" not in raw_value.lower():
        return f"{value:.1f}"
    return raw_value


def normalize_value_for_field(field: EditableField, raw_value: str) -> str:
    value = raw_value.strip()
    if not value:
        raise ValueError("Value cannot be empty.")

    match field.value_type:
        case "int":
            if not INT_RE.fullmatch(value):
                raise ValueError("Expected an integer value.")
            return value
        case "float":
            if not FLOAT_RE.fullmatch(value):
                raise ValueError("Expected a float value.")
            return normalize_float_string(value)
        case "sid":
            if value.startswith("#"):
                if not SID_HASH_RE.fullmatch(value):
                    raise ValueError("SID hash must look like #1A2B3C.")
                return value
            if value.startswith("0x"):
                raise ValueError("SID fields do not accept pointer syntax.")
            return value
        case "pointer":
            if not HEX_RE.fullmatch(value):
                raise ValueError("Expected a pointer address like 0x11C28.")
            return value.lower()
        case _:
            raise ValueError(f"Unsupported field type: {field.value_type}")


def classify_value_type(raw_type: str) -> str | None:
    normalized = raw_type.strip().lower()
    if normalized == "int":
        return "int"
    if normalized == "float":
        return "float"
    if normalized == "sid":
        return "sid"
    return None


def parse_asm_document(path: Path, text: str) -> AsmDocument:
    lines = text.splitlines()
    editable_fields: dict[int, EditableField] = {}
    source_bin_path: Path | None = None
    game_type = "t1x"
    stack: list[ContainerContext] = []

    for index, line in enumerate(lines, start=1):
        if not source_bin_path:
            header_match = HEADER_FILE_RE.match(line)
            if header_match:
                source_bin_path = resolve_listing_path(header_match.group("path"))
        game_match = HEADER_GAME_RE.match(line)
        if game_match:
            parsed_game = game_match.group("game").lower()
            if parsed_game in DEFAULT_GAMES:
                game_type = parsed_game

        if line.strip() == "}":
            if stack:
                stack.pop()
            continue

        root_match = ROOT_CONTAINER_RE.match(line)
        if root_match:
            stack.append(
                ContainerContext(
                    address=root_match.group("address"),
                    depth=len(root_match.group("indent")),
                    label=root_match.group("label").strip(),
                )
            )
            continue

        member_container_match = MEMBER_CONTAINER_RE.match(line)
        if member_container_match:
            if stack:
                member_index = int(member_container_match.group("member"))
                address = member_container_match.group("address")
                raw_type = member_container_match.group("type").strip()
                editable_fields[index] = EditableField(
                    line_number=index,
                    member_index=member_index,
                    parent_address=stack[-1].address,
                    value_type="pointer",
                    old_value=address,
                    line_text=line,
                    display_name=f"[{member_index}] {raw_type}",
                    raw_type=raw_type,
                )
            stack.append(
                ContainerContext(
                    address=member_container_match.group("address"),
                    depth=len(member_container_match.group("indent")),
                    label=member_container_match.group("type").strip(),
                )
            )
            continue

        value_match = VALUE_LINE_RE.match(line)
        if value_match and stack:
            member_index = int(value_match.group("member"))
            raw_type = value_match.group("type").strip()
            value_type = classify_value_type(raw_type)
            if value_type is None:
                continue
            editable_fields[index] = EditableField(
                line_number=index,
                member_index=member_index,
                parent_address=stack[-1].address,
                value_type=value_type,
                old_value=value_match.group("value").strip(),
                line_text=line,
                display_name=f"[{member_index}] {raw_type}",
                raw_type=raw_type,
            )

    return AsmDocument(
        display_path=path,
        source_bin_path=source_bin_path,
        asm_text=text,
        lines=lines,
        editable_fields=editable_fields,
        game_type=game_type,
    )


def build_disasm_command(
    dconstruct_path: Path,
    sidbase_path: Path,
    source_bin_path: Path,
    output_asm_path: Path,
    game_type: str,
    edit_expression: str | None = None,
) -> list[str]:
    command = [
        str(dconstruct_path),
        "-i",
        str(source_bin_path),
        "-o",
        str(output_asm_path),
        "-s",
        str(sidbase_path),
        "-g",
        game_type,
        "--no_decompile",
    ]
    if edit_expression:
        command.extend(["-e", edit_expression])
    return command


def run_dconstruct(command: list[str]) -> subprocess.CompletedProcess[str]:
    if not isinstance(command, list):
        raise TypeError("command must be a list of strings, not a shell string")
    return subprocess.run(command, capture_output=True, text=True, check=False, shell=False)


def format_process_failure(result: subprocess.CompletedProcess[str]) -> str:
    parts = [
        f"Command: {' '.join(result.args)}",
        f"Exit code: {result.returncode}",
    ]
    stdout = result.stdout.strip()
    stderr = result.stderr.strip()
    if stdout:
        parts.append(f"stdout:\n{stdout}")
    if stderr:
        parts.append(f"stderr:\n{stderr}")
    if not stdout and not stderr:
        parts.append("No output was captured from dconstruct.")
    return "\n\n".join(parts)


def make_temp_asm_path(source_bin_path: Path) -> Path:
    fd, raw_path = tempfile.mkstemp(
        prefix=f"{source_bin_path.stem}_",
        suffix=".asm",
    )
    os.close(fd)
    return Path(raw_path)


class AsmTextEdit(QPlainTextEdit):
    editable_line_clicked = pyqtSignal(int)

    def __init__(self) -> None:
        super().__init__()
        self._editable_lines: set[int] = set()
        self.setReadOnly(True)
        self.setLineWrapMode(QPlainTextEdit.LineWrapMode.NoWrap)
        self.setTabStopDistance(32)
        font = QFont("Consolas")
        font.setStyleHint(QFont.StyleHint.Monospace)
        font.setPointSize(10)
        self.setFont(font)

    def set_editable_lines(self, lines: Iterable[int]) -> None:
        self._editable_lines = set(lines)

    def mousePressEvent(self, event) -> None:  # type: ignore[override]
        cursor = self.cursorForPosition(event.pos())
        line_number = cursor.blockNumber() + 1
        if line_number in self._editable_lines:
            self.editable_line_clicked.emit(line_number)
        super().mousePressEvent(event)

    def mouseMoveEvent(self, event) -> None:  # type: ignore[override]
        cursor = self.cursorForPosition(event.pos())
        line_number = cursor.blockNumber() + 1
        viewport = self.viewport()
        if line_number in self._editable_lines:
            viewport.setCursor(Qt.CursorShape.PointingHandCursor)
        else:
            viewport.unsetCursor()
        super().mouseMoveEvent(event)


class SearchResultsDialog(QDialog):
    def __init__(self, hits: list[SearchHit], on_open_hit, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self.setWindowTitle("Search Results")
        self.resize(900, 500)
        self._hits = hits
        self._on_open_hit = on_open_hit

        layout = QVBoxLayout(self)
        self.list_widget = QListWidget(self)
        for hit in hits:
            item = QListWidgetItem(f"{hit.title}:{hit.line_number}  {hit.line_text.strip()}")
            self.list_widget.addItem(item)
        self.list_widget.itemDoubleClicked.connect(self.open_selected_hit)
        layout.addWidget(self.list_widget)

    def open_selected_hit(self, *_args) -> None:
        row = self.list_widget.currentRow()
        if row < 0:
            return
        self._on_open_hit(self._hits[row])
        self.accept()


class AsmTab(QWidget):
    field_selected = pyqtSignal(object)

    def __init__(self, document: AsmDocument) -> None:
        super().__init__()
        self.document = document
        self.selected_field: EditableField | None = None

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)

        self.editor = AsmTextEdit()
        self.editor.setPlainText(document.asm_text)
        self.editor.set_editable_lines(document.editable_fields.keys())
        self.editor.editable_line_clicked.connect(self.select_line)
        layout.addWidget(self.editor)

    def select_line(self, line_number: int) -> None:
        field = self.document.editable_fields.get(line_number)
        if not field:
            return
        self.selected_field = field
        self.highlight_line(line_number)
        self.field_selected.emit(field)

    def highlight_line(self, line_number: int) -> None:
        cursor = QTextCursor(self.editor.document().findBlockByLineNumber(line_number - 1))
        selection = QTextEdit.ExtraSelection()
        selection.cursor = cursor
        selection.cursor.clearSelection()
        highlight_color = self.editor.palette().color(QPalette.ColorRole.Highlight).lighter(165)
        highlight_color.setAlpha(110)
        selection.format.setBackground(highlight_color)
        selection.format.setProperty(QTextFormat.Property.FullWidthSelection, True)
        self.editor.setExtraSelections([selection])
        self.editor.setTextCursor(cursor)
        self.editor.centerCursor()

    def goto_line(self, line_number: int) -> None:
        self.highlight_line(line_number)
        self.select_line(line_number)

    def find_next(self, query: str, backwards: bool = False) -> bool:
        if not query:
            return False
        flags = QTextDocument.FindFlag.FindBackward if backwards else QTextDocument.FindFlag(0)
        if self.editor.find(query, flags):
            return True

        cursor = self.editor.textCursor()
        cursor.movePosition(QTextCursor.MoveOperation.End if backwards else QTextCursor.MoveOperation.Start)
        self.editor.setTextCursor(cursor)
        return self.editor.find(query, flags)

    def replace_document(self, document: AsmDocument) -> None:
        self.document = document
        self.selected_field = None
        self.editor.setPlainText(document.asm_text)
        self.editor.set_editable_lines(document.editable_fields.keys())
        self.editor.setExtraSelections([])


class MainWindow(QMainWindow):
    def __init__(self, startup_paths: list[Path]) -> None:
        super().__init__()
        self.setWindowTitle("dconstruct Interactive Editor")
        self.resize(1500, 920)
        self.settings = QSettings("DeepQuantum", "dconstruct-asm-editor")
        self.search_dialog: SearchResultsDialog | None = None
        self._dconstruct_overridden = False
        self._sidbase_overridden = False

        self.tabs = QTabWidget(self)
        self.tabs.setTabsClosable(True)
        self.tabs.tabCloseRequested.connect(self.close_tab)
        self.tabs.currentChanged.connect(self.on_current_tab_changed)

        self.dconstruct_input = QLineEdit(self)
        self.sidbase_input = QLineEdit(self)
        self.game_combo = QComboBox(self)
        self.game_combo.addItems(DEFAULT_GAMES)
        self.search_input = QLineEdit(self)
        self.search_input.setPlaceholderText("Search current tab or all open tabs")
        self.value_input = QLineEdit(self)

        self.selected_file_label = QLabel("No file selected", self)
        self.selected_field_label = QLabel("Click an editable field in the listing.", self)
        self.selected_old_value_label = QLabel("-", self)
        self.selected_type_label = QLabel("-", self)
        self.selected_expr_label = QLabel("-", self)

        self._build_ui()
        self._load_settings()

        for path in startup_paths:
            self.open_path(path)

    def create_loading_dialog(self, title: str, label_text: str) -> QProgressDialog:
        dialog = QProgressDialog(label_text, "", 0, 0, self)
        dialog.setWindowTitle(title)
        dialog.setWindowModality(Qt.WindowModality.ApplicationModal)
        dialog.setCancelButton(None)
        dialog.setMinimumDuration(0)
        dialog.setAutoClose(False)
        dialog.setAutoReset(False)
        dialog.setValue(0)
        dialog.show()
        QApplication.processEvents()
        return dialog

    def _build_ui(self) -> None:
        open_action = QAction("Open Files", self)
        open_action.triggered.connect(self.open_files_dialog)
        refresh_action = QAction("Refresh Current Tab", self)
        refresh_action.triggered.connect(self.refresh_current_tab)

        file_menu = self.menuBar().addMenu("File")
        file_menu.addAction(open_action)
        file_menu.addAction(refresh_action)

        central = QWidget(self)
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)

        config_group = QGroupBox("Tool Settings", self)
        config_layout = QGridLayout(config_group)
        config_layout.addWidget(QLabel("dconstruct.exe"), 0, 0)
        config_layout.addWidget(self.dconstruct_input, 0, 1)
        browse_exe_button = QPushButton("Browse", self)
        browse_exe_button.clicked.connect(self.browse_dconstruct)
        config_layout.addWidget(browse_exe_button, 0, 2)

        config_layout.addWidget(QLabel("sidbase.bin"), 1, 0)
        config_layout.addWidget(self.sidbase_input, 1, 1)
        browse_sid_button = QPushButton("Browse", self)
        browse_sid_button.clicked.connect(self.browse_sidbase)
        config_layout.addWidget(browse_sid_button, 1, 2)

        config_layout.addWidget(QLabel("Default Game"), 2, 0)
        config_layout.addWidget(self.game_combo, 2, 1)
        open_button = QPushButton("Open Files", self)
        open_button.clicked.connect(self.open_files_dialog)
        config_layout.addWidget(open_button, 2, 2)
        layout.addWidget(config_group)

        search_group = QGroupBox("Search", self)
        search_layout = QHBoxLayout(search_group)
        search_layout.addWidget(self.search_input)

        find_next_button = QPushButton("Next", self)
        find_next_button.clicked.connect(lambda: self.find_in_current_tab(backwards=False))
        search_layout.addWidget(find_next_button)

        find_prev_button = QPushButton("Prev", self)
        find_prev_button.clicked.connect(lambda: self.find_in_current_tab(backwards=True))
        search_layout.addWidget(find_prev_button)

        find_all_button = QPushButton("All Tabs", self)
        find_all_button.clicked.connect(self.search_all_tabs)
        search_layout.addWidget(find_all_button)
        layout.addWidget(search_group)

        layout.addWidget(self.tabs, stretch=1)

        edit_group = QGroupBox("Selected Field", self)
        edit_layout = QFormLayout(edit_group)
        edit_layout.addRow("Source Bin", self.selected_file_label)
        edit_layout.addRow("Field", self.selected_field_label)
        edit_layout.addRow("Current Value", self.selected_old_value_label)
        edit_layout.addRow("Type", self.selected_type_label)
        edit_layout.addRow("Edit", self.selected_expr_label)

        input_row = QHBoxLayout()
        self.value_input.setPlaceholderText("Enter replacement value")
        input_row.addWidget(self.value_input, stretch=1)
        apply_button = QPushButton("Apply Edit", self)
        apply_button.clicked.connect(self.apply_current_edit)
        input_row.addWidget(apply_button)
        refresh_button = QPushButton("Refresh Tab", self)
        refresh_button.clicked.connect(self.refresh_current_tab)
        input_row.addWidget(refresh_button)
        edit_layout.addRow("New Value", input_row)
        layout.addWidget(edit_group)

        status = QStatusBar(self)
        self.setStatusBar(status)

    def _load_settings(self) -> None:
        default_exe = detect_default_dconstruct()
        stored_exe_override = bool(self.settings.value("dconstruct_path_override", False, type=bool))
        stored_exe = str(self.settings.value("dconstruct_path", ""))
        if stored_exe_override and stored_exe:
            self.dconstruct_input.setText(stored_exe)
            self._dconstruct_overridden = True
        else:
            self.dconstruct_input.setText(str(default_exe) if default_exe else "")
            self._dconstruct_overridden = False

        stored_sid_override = bool(self.settings.value("sidbase_path_override", False, type=bool))
        sidbase_value = str(self.settings.value("sidbase_path", ""))
        if stored_sid_override and sidbase_value:
            self.sidbase_input.setText(sidbase_value)
            self._sidbase_overridden = True
        elif default_exe is not None:
            self.sidbase_input.setText(str(sidbase_next_to_dconstruct(default_exe)))
            self._sidbase_overridden = False

        game_value = str(self.settings.value("game_type", "t1x")).lower()
        index = self.game_combo.findText(game_value)
        if index >= 0:
            self.game_combo.setCurrentIndex(index)

    def closeEvent(self, event) -> None:  # type: ignore[override]
        self.settings.setValue("dconstruct_path_override", self._dconstruct_overridden)
        self.settings.setValue("sidbase_path_override", self._sidbase_overridden)
        self.settings.setValue(
            "dconstruct_path",
            self.dconstruct_input.text().strip() if self._dconstruct_overridden else "",
        )
        self.settings.setValue(
            "sidbase_path",
            self.sidbase_input.text().strip() if self._sidbase_overridden else "",
        )
        self.settings.setValue("game_type", self.game_combo.currentText())
        super().closeEvent(event)

    def browse_dconstruct(self) -> None:
        file_path, _ = QFileDialog.getOpenFileName(
            self,
            "Select dconstruct executable",
            self.dconstruct_input.text() or str(ROOT),
            "Executable (*.exe)",
        )
        if file_path:
            self.dconstruct_input.setText(file_path)
            self._dconstruct_overridden = True
            if not self._sidbase_overridden:
                self.sidbase_input.setText(str(sidbase_next_to_dconstruct(Path(file_path))))

    def browse_sidbase(self) -> None:
        file_path, _ = QFileDialog.getOpenFileName(
            self,
            "Select sidbase file",
            self.sidbase_input.text() or str(ROOT),
            "SIDBase (*.bin);;All Files (*)",
        )
        if file_path:
            self.sidbase_input.setText(file_path)
            self._sidbase_overridden = True

    def open_files_dialog(self) -> None:
        file_paths, _ = QFileDialog.getOpenFileNames(
            self,
            "Open BIN files",
            str(ROOT),
            "BIN Files (*.bin);;All Files (*)",
        )
        for raw_path in file_paths:
            self.open_path(Path(raw_path))

    def open_path(self, path: Path) -> None:
        loading_dialog = self.create_loading_dialog("Loading", f"Loading {path.name}...")
        try:
            if path.suffix.lower() == ".bin":
                document = self.disassemble_bin_to_document(path, self.game_combo.currentText())
            else:
                raise ValueError("Only raw .bin files are supported.")
        except Exception as exc:
            loading_dialog.close()
            self.show_error("Open failed", str(exc))
            return
        loading_dialog.close()

        tab = AsmTab(document)
        tab.field_selected.connect(self.on_field_selected)
        title = path.name
        self.tabs.addTab(tab, title)
        self.tabs.setCurrentWidget(tab)
        self.statusBar().showMessage(f"Opened {path}", 4000)

    def current_tab(self) -> AsmTab | None:
        widget = self.tabs.currentWidget()
        if isinstance(widget, AsmTab):
            return widget
        return None

    def close_tab(self, index: int) -> None:
        widget = self.tabs.widget(index)
        self.tabs.removeTab(index)
        if widget is not None:
            widget.deleteLater()
        if self.tabs.count() == 0:
            self.clear_selected_field()

    def on_current_tab_changed(self, _index: int) -> None:
        tab = self.current_tab()
        if tab and tab.selected_field:
            self.on_field_selected(tab.selected_field)
        else:
            self.clear_selected_field()

    def clear_selected_field(self) -> None:
        self.selected_file_label.setText("No file selected")
        self.selected_field_label.setText("Click an editable field in the listing.")
        self.selected_old_value_label.setText("-")
        self.selected_type_label.setText("-")
        self.selected_expr_label.setText("-")
        self.value_input.clear()

    def on_field_selected(self, field: EditableField) -> None:
        tab = self.current_tab()
        source_bin = tab.document.source_bin_path if tab else None
        self.selected_file_label.setText(str(source_bin) if source_bin else "Unknown")
        self.selected_field_label.setText(field.display_name)
        self.selected_old_value_label.setText(field.old_value)
        self.selected_type_label.setText(field.value_type)
        self.selected_expr_label.setText(field.edit_expression_prefix)
        self.value_input.setText(field.old_value)
        self.statusBar().showMessage(
            f"Selected {field.display_name} at line {field.line_number}",
            3000,
        )

    def get_validated_tool_paths(self) -> tuple[Path, Path]:
        dconstruct_raw = self.dconstruct_input.text().strip()
        dconstruct_path = Path(dconstruct_raw) if dconstruct_raw else detect_default_dconstruct()
        if dconstruct_path is None or not dconstruct_path.exists():
            raise FileNotFoundError("Could not find dconstruct.exe. Put it in PATH or configure it explicitly.")
        sidbase_raw = self.sidbase_input.text().strip()
        sidbase_path = Path(sidbase_raw) if sidbase_raw else sidbase_next_to_dconstruct(dconstruct_path)
        if not sidbase_path.exists():
            raise FileNotFoundError(f"Could not find sidbase.bin at {sidbase_path}.")
        return dconstruct_path, sidbase_path

    def disassemble_bin_to_document(self, bin_path: Path, game_type: str) -> AsmDocument:
        dconstruct_path, sidbase_path = self.get_validated_tool_paths()
        temp_asm_path = make_temp_asm_path(bin_path)
        try:
            command = build_disasm_command(
                dconstruct_path=dconstruct_path,
                sidbase_path=sidbase_path,
                source_bin_path=bin_path,
                output_asm_path=temp_asm_path,
                game_type=game_type,
            )
            result = run_dconstruct(command)
            if result.returncode != 0:
                raise RuntimeError(format_process_failure(result))
            text = temp_asm_path.read_text(encoding="utf-8")
            document = parse_asm_document(bin_path.with_suffix(bin_path.suffix + ".asm"), text)
            document.source_bin_path = bin_path
            document.game_type = game_type
            return document
        finally:
            temp_asm_path.unlink(missing_ok=True)

    def refresh_current_tab(self) -> None:
        tab = self.current_tab()
        if not tab:
            return
        source_bin = tab.document.source_bin_path
        if source_bin is None:
            self.show_error("Refresh failed", "The source .bin path could not be resolved from this tab.")
            return
        try:
            refreshed = self.disassemble_bin_to_document(source_bin, tab.document.game_type)
        except Exception as exc:
            self.show_error("Refresh failed", str(exc))
            return
        tab.replace_document(refreshed)
        self.clear_selected_field()
        self.statusBar().showMessage(f"Refreshed {source_bin.name}", 3000)

    def apply_current_edit(self) -> None:
        tab = self.current_tab()
        if not tab or not tab.selected_field:
            self.show_error("No field selected", "Click an editable field first.")
            return

        field = tab.selected_field
        source_bin = tab.document.source_bin_path
        if source_bin is None:
            self.show_error("Edit failed", "The selected tab does not resolve back to a source .bin file.")
            return

        try:
            dconstruct_path, sidbase_path = self.get_validated_tool_paths()
            new_value = normalize_value_for_field(field, self.value_input.text())
        except Exception as exc:
            self.show_error("Invalid edit", str(exc))
            return

        edit_expression = f"{field.edit_expression_prefix}{new_value}"
        confirmation = QMessageBox.question(
            self,
            "Replace Original Binary",
            f"Apply this edit and replace the original file?\n\n{source_bin}\n\n{edit_expression}",
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No,
            QMessageBox.StandardButton.No,
        )
        if confirmation != QMessageBox.StandardButton.Yes:
            return

        temp_asm_path = make_temp_asm_path(source_bin)
        edited_bin_path = source_bin.with_name(f"{source_bin.stem}_edited.bin")
        try:
            command = build_disasm_command(
                dconstruct_path=dconstruct_path,
                sidbase_path=sidbase_path,
                source_bin_path=source_bin,
                output_asm_path=temp_asm_path,
                game_type=tab.document.game_type,
                edit_expression=edit_expression,
            )
            result = run_dconstruct(command)
            if result.returncode != 0:
                raise RuntimeError(format_process_failure(result))
            if not edited_bin_path.exists():
                details = format_process_failure(result)
                raise RuntimeError(f"Expected edited output at {edited_bin_path}, but it was not created.\n\n{details}")

            os.replace(edited_bin_path, source_bin)

            updated_text = temp_asm_path.read_text(encoding="utf-8")
            updated_document = parse_asm_document(tab.document.display_path, updated_text)
            updated_document.source_bin_path = source_bin
            updated_document.game_type = tab.document.game_type
            tab.replace_document(updated_document)
            self.clear_selected_field()
            self.statusBar().showMessage(f"Applied {edit_expression}", 5000)

            output = (result.stdout + "\n" + result.stderr).strip()
            if output:
                QMessageBox.information(self, "Edit applied", output)
        except Exception as exc:
            self.show_error("Edit failed", str(exc))
        finally:
            temp_asm_path.unlink(missing_ok=True)
            edited_bin_path.unlink(missing_ok=True)

    def find_in_current_tab(self, backwards: bool) -> None:
        tab = self.current_tab()
        if not tab:
            return
        query = self.search_input.text().strip()
        if not query:
            return
        found = tab.find_next(query, backwards=backwards)
        if not found:
            self.statusBar().showMessage(f"No matches for '{query}'", 3000)

    def search_all_tabs(self) -> None:
        query = self.search_input.text().strip().lower()
        if not query:
            return

        hits: list[SearchHit] = []
        for index in range(self.tabs.count()):
            widget = self.tabs.widget(index)
            if not isinstance(widget, AsmTab):
                continue
            for line_number, line_text in enumerate(widget.document.lines, start=1):
                if query in line_text.lower():
                    hits.append(
                        SearchHit(
                            tab_index=index,
                            title=self.tabs.tabText(index),
                            line_number=line_number,
                            line_text=line_text,
                        )
                    )

        if not hits:
            self.statusBar().showMessage(f"No matches for '{query}'", 3000)
            return

        dialog = SearchResultsDialog(hits, self.open_search_hit, self)
        dialog.exec()

    def open_search_hit(self, hit: SearchHit) -> None:
        self.tabs.setCurrentIndex(hit.tab_index)
        tab = self.current_tab()
        if not tab:
            return
        tab.goto_line(hit.line_number)

    def show_error(self, title: str, message: str) -> None:
        QMessageBox.critical(self, title, message)
        self.statusBar().showMessage(message, 5000)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Interactive dconstruct ASM editor.")
    parser.add_argument("paths", nargs="*", help="Optional .asm or .bin files to open on startup.")
    parser.add_argument("--dconstruct", dest="dconstruct_path", help="Path to dconstruct.exe")
    parser.add_argument("--sidbase", dest="sidbase_path", help="Path to sidbase.bin")
    parser.add_argument("--game-type", choices=DEFAULT_GAMES, help="Default game type for opening raw .bin files.")
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv if argv is not None else sys.argv[1:])

    app = QApplication(sys.argv)
    available_styles = {style.lower(): style for style in QStyleFactory.keys()}
    preferred_style = available_styles.get("windowsvista") or available_styles.get("windows")
    if preferred_style:
        app.setStyle(preferred_style)
    app.setPalette(app.style().standardPalette())
    app.setStyleSheet("")

    startup_paths = [Path(path).expanduser().resolve() for path in args.paths]
    window = MainWindow(startup_paths)
    if args.dconstruct_path:
        window.dconstruct_input.setText(args.dconstruct_path)
        window._dconstruct_overridden = True
    if args.sidbase_path:
        window.sidbase_input.setText(args.sidbase_path)
        window._sidbase_overridden = True
    if args.game_type:
        index = window.game_combo.findText(args.game_type)
        if index >= 0:
            window.game_combo.setCurrentIndex(index)

    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
