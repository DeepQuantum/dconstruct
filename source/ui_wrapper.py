import tkinter as tk
from tkinter import filedialog, messagebox, ttk


class CLIWrapperApp:
    def __init__(self, root):
        self.root = root
        root.title("CLI Wrapper")
        root.geometry("500x400")

        self.input_is_dir = tk.BooleanVar(value=False)
        self.emit_once = tk.BooleanVar(value=False)

        self.input_path = tk.StringVar()
        self.edit_file = tk.StringVar()
        self.s_file = tk.StringVar()
        self.output_path = tk.StringVar()
        self.edit_types = tk.StringVar()
        self.indent_level = tk.StringVar()

        self.create_widgets()

    def create_widgets(self):
        row = 0

        tk.Label(self.root, text="Input (-i):").grid(row=row, column=0, sticky="w", padx=5, pady=5)
        tk.Button(self.root, text="Choose...", command=self.choose_input).grid(row=row, column=1, padx=5, pady=5)
        ttk.Checkbutton(self.root, text="Is Directory?", variable=self.input_is_dir).grid(row=row, column=2, padx=5)
        row += 1

        tk.Label(self.root, text="Edit file:").grid(row=row, column=0, sticky="w", padx=5)
        tk.Button(self.root, text="Choose File", command=lambda: self.pick_file(self.edit_file)).grid(row=row, column=1, padx=5)
        row += 1

        tk.Label(self.root, text="-s:").grid(row=row, column=0, sticky="w", padx=5)
        tk.Button(self.root, text="Choose File", command=lambda: self.pick_file(self.s_file)).grid(row=row, column=1, padx=5)
        row += 1

        tk.Label(self.root, text="-o Output:").grid(row=row, column=0, sticky="w", padx=5)
        tk.Button(self.root, text="Choose...", command=self.choose_output).grid(row=row, column=1, padx=5)
        row += 1

        tk.Label(self.root, text="-e (comma-separated):").grid(row=row, column=0, sticky="w", padx=5)
        tk.Text(self.root).grid(row=row, column=1, columnspan=2, padx=5, sticky="ew")
        row += 1

        tk.Label(self.root, text="--indent:").grid(row=row, column=0, sticky="w", padx=5)
        tk.Entry(self.root, textvariable=self.indent_level).grid(row=row, column=1, padx=5)
        row += 1

        ttk.Checkbutton(self.root, text="--emit_once", variable=self.emit_once).grid(row=row, column=0, sticky="w", padx=5)
        row += 1

        tk.Button(self.root, text="Run", command=self.run_command).grid(row=row, column=0, columnspan=3, pady=20)

    def choose_input(self):
        if self.input_is_dir.get():
            path = filedialog.askdirectory()
        else:
            path = filedialog.askopenfilename()
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
