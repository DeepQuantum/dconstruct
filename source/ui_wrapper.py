import tkinter as tk
from tkinter import filedialog, messagebox, ttk


class CLIWrapperApp:
    def __init__(self, root):
        self.root = root
        root.title("CLI Wrapper")
        root.geometry("900x600")

        self.input_is_dir = tk.BooleanVar(value=False)
        self.emit_once = tk.BooleanVar(value=False)

        self.input_path = tk.StringVar()
        self.edit_file = tk.StringVar()
        self.s_file = tk.StringVar()
        self.output_path = tk.StringVar()
        self.edit_types = tk.StringVar()
        self.indent_level = tk.IntVar()

        self.create_widgets()

    def create_widgets(self):
        row = 0

        tk.Label(self.root, text=".bin File(s) to disassemble (choose a file or directory):").grid(row=row, column=0, sticky="w", padx=5, pady=20)
        self.input_path_var = tk.StringVar()
        tk.Entry(self.root, textvariable=self.input_path_var, width=60).grid(row=row, column=1, padx=5)
        tk.Button(self.root, text="📁", command=self.choose_input_files).grid(row=row, column=2, padx=5)
        tk.Button(self.root, text="📂", command=self.choose_input_dir).grid(row=row, column=3, padx=5)
        row += 1

        tk.Label(self.root, text="Output directory: ").grid(row=row, column=0, sticky="w", padx=5)
        self.output_path_var = tk.StringVar()
        tk.Entry(self.root, textvariable=self.output_path_var, width=60).grid(row=row, column=1, padx=5)
        tk.Button(self.root, text="📁", command=self.choose_output).grid(row=row, column=2, padx=5)
        row += 1

        tk.Label(self.root, text="Sidbase path (default is 'sidbase.bin' in the current directory):").grid(row=row, column=0, sticky="w", padx=5)
        self.sidbase_path_var = tk.StringVar()
        tk.Entry(self.root, textvariable=self.sidbase_path_var, width=60).grid(row=row, column=1, padx=5)
        tk.Button(self.root, text="📁", command=lambda: self.pick_file(self.sidbase_path_var)).grid(row=row, column=2, padx=5)
        row += 1

        tk.Label(self.root, text="Edit file:").grid(row=row, column=0, sticky="w", padx=5)
        self.edit_file_path_var = tk.StringVar()
        tk.Entry(self.root, textvariable=self.edit_file_path_var, width=60).grid(row=row, column=1, padx=5)
        tk.Button(self.root, text="📁", command=lambda: self.pick_file(self.edit_file_path_var)).grid(row=row, column=2, padx=5)
        row += 1

        tk.Label(self.root, text="Add any additional edits (optional):").grid(row=row, column=0, sticky="w", padx=5)
        self.additional_edits_text = tk.Text(self.root, height=4, width=60)
        self.additional_edits_text.grid(row=row, column=1, columnspan=2, padx=5, sticky="ew")
        row += 1

        tk.Label(self.root, text="--indent:").grid(row=row, column=0, sticky="w", padx=5)
        tk.Entry(self.root, textvariable=self.indent_level).grid(row=row, column=1, padx=5)
        row += 1

        ttk.Checkbutton(self.root, text="--emit_once", variable=self.emit_once).grid(row=row, column=0, sticky="w", padx=5)
        row += 1

        tk.Button(self.root, text="Run", command=self.run_command).grid(row=row, column=0, columnspan=3, pady=20)

    def choose_input_files(self):
        path = filedialog.askopenfilenames()
        if path:
            self.input_path.set(path)

    def choose_input_dir(self):
        path = filedialog.askdirectory()
        if path:
            self.input_path.set(path)

    def choose_output(self):
        if self.input_is_dir.get():
            path = filedialog.askdirectory()
        else:
            path = filedialog.asksaveasfilename()
        if path:
            self.output_path.set(path)

    def pick_file(self, var):
        path = filedialog.askopenfilename()
        if path:
            var.set(path)

    def run_command(self):
        cmd = ["cli_command_placeholder"]

        if self.input_path.get():
            cmd.extend(["-i", self.input_path.get()])
        else:
            messagebox.showerror("Missing Input", "Please select an input.")
            return

        if self.edit_file.get():
            cmd.extend(["--edit_file", self.edit_file.get()])

        if self.s_file.get():
            cmd.extend(["-s", self.s_file.get()])

        if self.output_path.get():
            cmd.extend(["-o", self.output_path.get()])
        else:
            messagebox.showerror("Missing Output", "Please select an output.")
            return

        edits = [e.strip() for e in self.edit_types.get().split(",") if e.strip()]
        for e in edits:
            cmd.extend(["-e", e])

        if self.indent_level.get():
            try:
                indent = int(self.indent_level.get())
                cmd.extend(["--indent", str(indent)])
            except ValueError:
                messagebox.showerror("Invalid Indent", "Indent must be a number.")
                return

        if self.emit_once.get():
            cmd.append("--emit_once")

        print("Running command:")
        print(" ".join(cmd))
        messagebox.showinfo("Command Built", "Command:\n" + " ".join(cmd))


if __name__ == "__main__":
    root = tk.Tk()
    app = CLIWrapperApp(root)
    root.mainloop()
