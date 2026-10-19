import os
import shutil
import struct
import hashlib
import tkinter as tk
from tkinter import filedialog, messagebox, ttk, scrolledtext


# Simple Tooltip Helper Class for Hover Descriptions
class ToolTip:

  def __init__(self, widget, text):
    self.widget = widget
    self.text = text
    self.tipwindow = None
    self.widget.bind("<Enter>", self.show_tip)
    self.widget.bind("<Leave>", self.hide_tip)

  def show_tip(self, event=None):
    if self.tipwindow or not self.text:
      return
    x = self.widget.winfo_rootx() + 20
    y = self.widget.winfo_rooty() + self.widget.winfo_height() + 5
    self.tipwindow = tw = tk.Toplevel(self.widget)
    tw.wm_overrideredirect(True)
    tw.wm_geometry(f"+{x}+{y}")
    label = tk.Label(
        tw,
        text=self.text,
        justify=tk.LEFT,
        background="#2c3e50",
        foreground="#ffffff",
        relief=tk.SOLID,
        borderwidth=1,
        font=("Arial", 9),
        padx=5,
        pady=3,
    )
    label.pack(ipadx=1, ipady=1)

  def hide_tip(self, event=None):
    tw = self.tipwindow
    self.tipwindow = None
    if tw:
      tw.destroy()


class DayZPBOPackerApp:

  def __init__(self, root):
    self.root = root
    self.root.title("Linux DayZ PBO Packer & Folder Cleaner")
    self.root.geometry("700x620")
    self.root.minsize(620, 520)

    # Create Notebook (Tabs)
    self.notebook = ttk.Notebook(self.root)
    self.notebook.pack(fill="both", expand=True, padx=10, pady=10)

    # Tab 1: Basic / Guided Mode
    self.tab_basic = ttk.Frame(self.notebook)
    self.notebook.add(self.tab_basic, text=" Basic / Guided Mode ")

    # Tab 2: Normal Mode
    self.tab_normal = ttk.Frame(self.notebook)
    self.notebook.add(self.tab_normal, text=" Normal Mode ")

    # Build UI tabs
    self.build_basic_tab()
    self.build_normal_tab()

  # ------------------------------------------------------------------
  # TAB 1: BASIC / GUIDED MODE
  # ------------------------------------------------------------------
  def build_basic_tab(self):
    frame = self.tab_basic
    frame.columnconfigure(1, weight=1)

    # Explanation Banner
    info_txt = (
        "Guided Mode inspects your mod folder layout, warns about nested"
        " traps,\nand can automatically clean/flatten mis-nested folders on your"
        " disk!"
    )
    banner = tk.Label(
        frame,
        text=info_txt,
        bg="#2c3e50",
        fg="white",
        justify="left",
        padx=10,
        pady=8,
    )
    banner.grid(row=0, column=0, columnspan=3, sticky="ew", padx=10, pady=10)

    # Folder Picker
    lbl_src = tk.Label(frame, text="Select Mod Folder:")
    lbl_src.grid(row=1, column=0, sticky="w", padx=10, pady=5)
    self.basic_folder_var = tk.StringVar()
    ent_src = tk.Entry(frame, textvariable=self.basic_folder_var)
    ent_src.grid(row=1, column=1, sticky="ew", padx=5, pady=5)
    btn_browse = tk.Button(
        frame, text="Browse...", command=self.browse_basic_folder
    )
    btn_browse.grid(row=1, column=2, padx=10, pady=5)

    ToolTip(
        ent_src,
        "The root folder of your mod project that you want to check and pack.",
    )
    ToolTip(btn_browse, "Click to open a folder browser to select your mod.")

    # Action Buttons Frame
    btn_frame = ttk.Frame(frame)
    btn_frame.grid(row=2, column=0, columnspan=3, padx=10, pady=8)

    self.btn_analyze = tk.Button(
        btn_frame,
        text="🔍 Analyze Folder Structure",
        bg="#3498db",
        fg="white",
        font=("Arial", 9, "bold"),
        command=self.analyze_structure,
    )
    self.btn_analyze.pack(side=tk.LEFT, padx=5)
    ToolTip(
        self.btn_analyze,
        "Scans your folder for config.cpp and nested folder mistakes.",
    )

    self.btn_fix = tk.Button(
        btn_frame,
        text="🛠️ Fix / Flatten Structure on Disk",
        bg="#e67e22",
        fg="white",
        font=("Arial", 9, "bold"),
        command=self.fix_folder_structure,
    )
    self.btn_fix.pack(side=tk.LEFT, padx=5)
    ToolTip(
        self.btn_fix,
        "Permanently reorganizes files on your hard drive to fix double-nested"
        " folder errors.",
    )

    # Structure Status Area
    self.status_box = scrolledtext.ScrolledText(
        frame, height=9, wrap=tk.WORD, bg="#1e1e1e", fg="#00ff00"
    )
    self.status_box.grid(
        row=3, column=0, columnspan=3, padx=10, pady=5, sticky="nsew"
    )
    frame.rowconfigure(3, weight=1)
    ToolTip(
        self.status_box,
        "Log window showing scan results, warnings, and error messages.",
    )

    # Detected PBO Target Folder & Prefix
    lbl_target = tk.Label(frame, text="Verified Target Folder:")
    lbl_target.grid(row=4, column=0, sticky="w", padx=10, pady=5)
    self.basic_target_var = tk.StringVar()
    self.ent_target = tk.Entry(
        frame, textvariable=self.basic_target_var, state="readonly"
    )
    self.ent_target.grid(
        row=4, column=1, columnspan=2, sticky="ew", padx=10, pady=5
    )
    ToolTip(
        self.ent_target,
        "The final automatically verified folder path that will be packed into"
        " the PBO.",
    )

    lbl_prefix = tk.Label(frame, text="PBO Prefix Name:")
    lbl_prefix.grid(row=5, column=0, sticky="w", padx=10, pady=5)
    self.basic_prefix_var = tk.StringVar()
    self.ent_prefix = tk.Entry(frame, textvariable=self.basic_prefix_var)
    self.ent_prefix.grid(
        row=5, column=1, columnspan=2, sticky="ew", padx=10, pady=5
    )
    ToolTip(
        self.ent_prefix,
        "The identifier prefix written inside the PBO header required by"
        " DayZ. Must match the PBO filename (e.g. UnderdogLeveler).",
    )

    # Pack Button
    self.btn_pack = tk.Button(
        frame,
        text="📦 Pack Verified PBO",
        bg="#2ecc71",
        fg="white",
        font=("Arial", 10, "bold"),
        command=self.pack_basic,
    )
    self.btn_pack.grid(row=6, column=0, columnspan=3, padx=10, pady=12)
    ToolTip(
        self.btn_pack,
        "Compiles the verified folder into a valid DayZ .pbo file.",
    )

  def browse_basic_folder(self):
    selected = filedialog.askdirectory(title="Select your DayZ Mod Project Folder")
    if selected:
      self.basic_folder_var.set(selected)
      self.analyze_structure()

  def analyze_structure(self):
    folder = self.basic_folder_var.get().strip()
    self.status_box.delete("1.0", tk.END)

    if not folder or not os.path.isdir(folder):
      self.status_box.insert(
          tk.END, "❌ Please select a valid directory first.\n"
      )
      return

    self.status_box.insert(
        tk.END, f"Analyzing: {folder}\n" + "-" * 50 + "\n"
    )

    subfolders = [
        f for f in os.listdir(folder) if os.path.isdir(os.path.join(folder, f))
    ]
    folder_name = os.path.basename(folder)

    target_dir = folder
    prefix = folder_name

    if "addons" in [s.lower() for s in subfolders]:
      addons_path = os.path.join(folder, "addons")
      addons_contents = [
          f
          for f in os.listdir(addons_path)
          if os.path.isdir(os.path.join(addons_path, f))
      ]
      self.status_box.insert(
          tk.END,
          "ℹ️ Found DayZ Root Mod structure containing an '/addons'"
          " directory.\n",
      )

      if addons_contents:
        target_dir = os.path.join(addons_path, addons_contents[0])
        prefix = os.path.basename(target_dir)
        self.status_box.insert(
            tk.END, f"✅ Auto-selected inner addon source: {target_dir}\n"
        )

    elif folder_name in subfolders:
      nested_path = os.path.join(folder, folder_name)
      self.status_box.insert(
          tk.END,
          f"⚠️ WARNING: Double-nested folder detected ('{folder_name}/"
          f"{folder_name}')!\n",
      )
      self.status_box.insert(
          tk.END,
          "💡 Click 'Fix / Flatten Structure on Disk' to permanently clean this"
          " up.\n",
      )
      target_dir = nested_path
      prefix = folder_name

    elif "config.cpp" in [f.lower() for f in os.listdir(folder)]:
      self.status_box.insert(
          tk.END,
          "✅ Clean DayZ Source Folder detected! 'config.cpp' found in root.\n",
      )
      target_dir = folder
      prefix = folder_name

    else:
      self.status_box.insert(
          tk.END,
          "⚠️ Note: 'config.cpp' not immediately visible in root. Review folder"
          " contents.\n",
      )

    self.basic_target_var.set(target_dir)
    self.basic_prefix_var.set(prefix)
    self.status_box.insert(
        tk.END, f"\nReady to pack PBO as: {prefix}.pbo\n"
    )

  def fix_folder_structure(self):
    folder = self.basic_folder_var.get().strip()
    if not folder or not os.path.isdir(folder):
      messagebox.showerror("Error", "Please select a valid folder first.")
      return

    subfolders = [
        f for f in os.listdir(folder) if os.path.isdir(os.path.join(folder, f))
    ]
    folder_name = os.path.basename(folder)

    if folder_name in subfolders:
      nested_path = os.path.join(folder, folder_name)
      nested_subfolders = os.listdir(nested_path)

      if "config.cpp" in [f.lower() for f in nested_subfolders] or len(
          nested_subfolders
      ) > 0:
        confirm = messagebox.askyesno(
            "Confirm Restructuring",
            f"Found duplicate nested folder:\n{nested_path}\n\nDo you want to"
            " move all inner files up one level and remove the wrapper folder?",
        )
        if not confirm:
          return

        try:
          self.status_box.insert(
              tk.END, "\n🛠️ Starting folder restructuring...\n"
          )
          temp_staging = os.path.join(folder, "_temp_staging_")

          shutil.move(nested_path, temp_staging)

          for item in os.listdir(temp_staging):
            src_item = os.path.join(temp_staging, item)
            dst_item = os.path.join(folder, item)
            if os.path.exists(dst_item):
              if os.path.isdir(dst_item):
                shutil.rmtree(dst_item)
              else:
                os.remove(dst_item)
            shutil.move(src_item, folder)

          shutil.rmtree(temp_staging)

          self.status_box.insert(
              tk.END, "✅ Success! Folder structure successfully flattened.\n"
          )
          messagebox.showinfo(
              "Success",
              "Folder has been successfully cleaned and flattened!",
          )
          self.analyze_structure()
          return

        except Exception as e:
          messagebox.showerror("Error", f"Failed to restructure folder: {e}")
          return

    messagebox.showinfo(
        "Info",
        "No standard double-nested folder structure detected that requires"
        " fixing.",
    )

  def pack_basic(self):
    target = self.basic_target_var.get()
    prefix = self.basic_prefix_var.get()
    if not target or not os.path.isdir(target):
      target = self.basic_folder_var.get().strip()
    if not prefix:
      prefix = os.path.basename(target)
    if not target or not os.path.isdir(target):
      messagebox.showerror("Error", "No valid target folder selected to pack!")
      return

    out_file = filedialog.asksaveasfilename(
        initialfile=f"{prefix}.pbo",
        defaultextension=".pbo",
        filetypes=[("PBO files", "*.pbo")],
    )
    if out_file:
      pack_pbo_binary(target, out_file, prefix)
      ok, info = verify_pbo(out_file)
      messagebox.showinfo(
          "Success",
          f"Packed DayZ PBO successfully!\n{out_file}\n{info}",
      )

  # ------------------------------------------------------------------
  # TAB 2: NORMAL MODE
  # ------------------------------------------------------------------
  def build_normal_tab(self):
    frame = self.tab_normal
    frame.columnconfigure(1, weight=1)

    tk.Label(frame, text="Source Path:").grid(
        row=0, column=0, sticky="w", padx=10, pady=10
    )
    self.norm_src = tk.StringVar()
    ent_nsrc = tk.Entry(frame, textvariable=self.norm_src)
    ent_nsrc.grid(row=0, column=1, sticky="ew", padx=5, pady=10)
    tk.Button(
        frame,
        text="Browse...",
        command=lambda: self.norm_src.set(filedialog.askdirectory()),
    ).grid(row=0, column=2, padx=10, pady=10)

    tk.Label(frame, text="Output File:").grid(
        row=1, column=0, sticky="w", padx=10, pady=10
    )
    self.norm_out = tk.StringVar()
    ent_nout = tk.Entry(frame, textvariable=self.norm_out)
    ent_nout.grid(row=1, column=1, sticky="ew", padx=5, pady=10)
    tk.Button(
        frame,
        text="Browse...",
        command=lambda: self.norm_out.set(
            filedialog.asksaveasfilename(defaultextension=".pbo")
        ),
    ).grid(row=1, column=2, padx=10, pady=10)

    tk.Label(frame, text="PBO Prefix:").grid(
        row=2, column=0, sticky="w", padx=10, pady=10
    )
    self.norm_prefix = tk.StringVar()
    ent_npre = tk.Entry(frame, textvariable=self.norm_prefix)
    ent_npre.grid(row=2, column=1, columnspan=2, sticky="ew", padx=10, pady=10)

    btn_npack = tk.Button(
        frame,
        text="Pack PBO Directly",
        bg="#2ecc71",
        fg="white",
        font=("Arial", 10, "bold"),
        command=self.pack_normal,
    )
    btn_npack.grid(row=3, column=0, columnspan=3, padx=10, pady=20)

    ToolTip(ent_nsrc, "Uncompressed source files folder.")
    ToolTip(ent_nout, "Destination path where the .pbo file will be created.")
    ToolTip(ent_npre, "The required virtual prefix value for the PBO.")
    ToolTip(btn_npack, "Immediately packs the source folder into a .pbo file.")

  def pack_normal(self):
    src, out, prefix = (
        self.norm_src.get(),
        self.norm_out.get(),
        self.norm_prefix.get(),
    )
    if not src or not os.path.isdir(src) or not out:
      messagebox.showerror(
          "Error", "Please provide valid source and output paths."
      )
      return
    if not prefix:
      prefix = os.path.basename(os.path.normpath(src))
    pack_pbo_binary(src, out, prefix)
    ok, info = verify_pbo(out)
    messagebox.showinfo("Success", f"PBO successfully packed.\n{info}")


# ----------------------------------------------------------------------
# HELPER: PBO BINARY BUILDER ENGINE (correct DayZ format)
# ----------------------------------------------------------------------
def pack_pbo_binary(src_dir, output_pbo_path, prefix_name):
  files = []
  for root, dirs, names in os.walk(src_dir):
    dirs[:] = [d for d in dirs if d not in ("__pycache__", ".git")]
    for name in names:
      ext = os.path.splitext(name)[1].lower()
      if name in ("README.md", "mod.cpp", "meta.cpp", "_unpbo_manifest.json") or ext in (
          ".md", ".pyc", ".pyo", ".bak", ".pbo"
      ):
        continue
      full = os.path.join(root, name)
      rel = os.path.relpath(full, src_dir).replace("/", "\\")
      files.append((rel, full))
  files.sort(key=lambda x: x[0].lower())

  buf = bytearray()

  # 1. "Vers" product entry: 0x00 + "sreV" + 16 zero bytes
  buf += b"\x00" + b"sreV" + b"\x00" * 16

  # 2. Prefix product entry
  buf += b"product\x00"
  buf += b"dayz ugc\x00"
  buf += b"prefix\x00"
  buf += prefix_name.encode("utf-8") + b"\x00"

  # 3. End of product entries
  buf += b"\x00"

  # 4. File entries: name\0 + [0, 0, 0, timestamp, dataSize]
  contents = []
  for rel, full in files:
    with open(full, "rb") as f:
      content = f.read()
    contents.append(content)
    buf += rel.encode("utf-8") + b"\x00"
    buf += struct.pack("<IIIII", 0, 0, 0, int(os.path.getmtime(full)), len(content))

  # 5. Terminating entry: empty filename + 5 zero fields (20 bytes)
  buf += b"\x00" + b"\x00" * 20

  # 6. File data
  for content in contents:
    buf += content

  # 7. Footer: null byte + SHA-1 of everything above it
  buf += b"\x00" + hashlib.sha1(bytes(buf)).digest()

  with open(output_pbo_path, "wb") as pbo:
    pbo.write(bytes(buf))


def verify_pbo(output_pbo_path):
  try:
    with open(output_pbo_path, "rb") as f:
      data = f.read()
  except Exception as e:
    return False, f"Could not read PBO: {e}"

  if len(data) < 22:
    return False, f"PBO too small ({len(data)} bytes) - packing likely failed."

  body_len = len(data) - 21
  if data[body_len] != 0:
    return False, "PBO footer missing."

  stored = data[body_len + 1:]
  calc = hashlib.sha1(data[:body_len]).digest()
  ok = (stored == calc)
  return ok, f"{len(data)} bytes, SHA-1 {'OK' if ok else 'MISMATCH'}"


if __name__ == "__main__":
  root = tk.Tk()
  app = DayZPBOPackerApp(root)
  root.mainloop()
