import ctypes
import os
import tkinter as tk
from tkinter import messagebox, ttk

# --- Nạp thư viện C ---
try:
    lib_path = os.path.abspath("jobfair.dll")
    jobfair = ctypes.CDLL(lib_path)
    jobfair.get_abbre.restype = ctypes.c_char_p
    jobfair.init_system()
except Exception as e:
    print(f"Lỗi nạp DLL: {e}")

enterprise_details = {}

def update_map_visuals():
    """Cập nhật lại màu sắc và chữ trên các nút gian hàng"""
    for i in range(30):
        if i in enterprise_details:
            abbre = enterprise_details[i]['abbre']
            buttons[i].config(text=abbre, bg="#2ecc71", fg="white")
        else:
            # Hiển thị số từ 1-30 trên nút
            buttons[i].config(text=str(i + 1), bg="#f0f0f0", fg="black")

def show_info(idx):
    """Popup hiển thị thông tin và thực hiện ALTER/DELETE"""
    info_win = tk.Toplevel(root)
    # Hiển thị số gian thực tế (idx + 1)
    info_win.title(f"Quản lý gian {idx + 1}")
    info_win.geometry("350x300")
    
    if idx in enterprise_details:
        data = enterprise_details[idx]
        tk.Label(info_win, text=f"THÔNG TIN DOANH NGHIỆP", font=("Arial", 11, "bold")).pack(pady=10)
        tk.Label(info_win, text=f"Tên: {data['name']}\nGiá trị: {data['val']} | Nặng: {data['weight']}kg", justify="left").pack(pady=5)
        
        # Chức năng XÓA (DELETE)
        def handle_delete():
            if messagebox.askyesno("Xác nhận", f"Bạn có chắc muốn xóa doanh nghiệp tại gian {idx + 1}?"):
                del enterprise_details[idx]
                update_map_visuals()
                info_win.destroy()
        
        tk.Button(info_win, text="XÓA DOANH NGHIỆP", bg="#e74c3c", fg="white", font=("Arial", 9, "bold"), command=handle_delete).pack(pady=10)
        
        tk.Frame(info_win, height=1, bg="grey").pack(fill="x", padx=20, pady=10)
        
        # Chức năng ĐỔI VỊ TRÍ (ALTER)
        tk.Label(info_win, text="Chuyển đến gian hàng số (1-30):").pack()
        ent_new = tk.Entry(info_win, width=10)
        ent_new.pack(pady=5)
        
        def handle_alter():
            try:
                # Nhận số từ 1-30 và chuyển về 0-29 cho C
                new_idx_user = int(ent_new.get())
                new_idx = new_idx_user - 1
                if new_idx < 0 or new_idx > 29: raise ValueError
                
                if new_idx in enterprise_details:
                    messagebox.showerror("Lỗi", "Gian hàng đích đã có người!")
                else:
                    enterprise_details[new_idx] = enterprise_details.pop(idx)
                    update_map_visuals()
                    info_win.destroy()
                    messagebox.showinfo("Thành công", f"Đã chuyển sang gian {new_idx + 1}")
            except: messagebox.showerror("Lỗi", "Vui lòng nhập số gian từ 1-30!")
            
        tk.Button(info_win, text="XÁC NHẬN ĐỔI CHỖ", bg="#3498db", fg="white", command=handle_alter).pack()
    else:
        tk.Label(info_win, text=f"Gian hàng {idx + 1} hiện đang trống.", font=("Arial", 10, "italic")).pack(pady=100)

def on_register():
    try:
        # Lấy dữ liệu và loại bỏ mọi khoảng trắng/ký tự thừa ở hai đầu
        name = entry_name.get().strip()
        val_raw = entry_val.get().strip()
        weight_raw = entry_weight.get().strip()
        booth_raw = entry_booth.get().strip()

        if not name:
            messagebox.showwarning("Chú ý", "Vui lòng nhập tên công ty!")
            return

        # Chuyển đổi an toàn
        val = int(val_raw)
        weight = int(weight_raw)
        
        # Xử lý số gian hàng (User nhập 1-30 -> C dùng 0-29)
        if booth_raw:
            target = int(booth_raw) - 1
            if target < 0 or target > 29:
                messagebox.showerror("Lỗi", "Gian hàng phải từ 1 đến 30!")
                return
        else:
            target = -1

        # Gọi hàm C[cite: 1]
        idx = jobfair.register_enterprise_gui(name.encode('utf-8'), target, val, weight)
        
        if idx != -1:
            abbre = jobfair.get_abbre(idx).decode('utf-8')
            enterprise_details[idx] = {'name': name, 'val': val, 'weight': weight, 'abbre': abbre}
            update_map_visuals()
            messagebox.showinfo("Thành công", f"Đã đăng ký tại gian {idx + 1}")
        else:
            messagebox.showerror("Lỗi", "Không thể đăng ký vị trí này!")

    except ValueError:
        # Thông báo lỗi rõ ràng hơn để bạn kiểm tra ô nào nhập sai[cite: 1]
        messagebox.showerror("Lỗi", "Giá trị, Khối lượng và Gian số phải là số nguyên thuần túy!")

def run_knapsack_v2():
    try:
        max_w = int(entry_max_w.get())
        total_val = jobfair.run_optimization(max_w)
        
        result_text.delete(1.0, tk.END)
        result_text.insert(tk.END, f"--- PHÂN TÍCH THUẬT TOÁN KNAPSACK ---\n")
        result_text.insert(tk.END, f"Giới hạn tải trọng: {max_w} kg\n")
        result_text.insert(tk.END, f"TỔNG GIÁ TRỊ ĐẠT ĐƯỢC: {total_val} ĐIỂM\n")
        result_text.insert(tk.END, "----------------------------------\n\n")
        result_text.insert(tk.END, "Danh sách doanh nghiệp trong phương án tối ưu:\n")
        
        # Liệt kê các doanh nghiệp có thể chứa được (hiển thị idx + 1 cho user)[cite: 1]
        for idx, data in sorted(enterprise_details.items()):
            if data['weight'] <= max_w:
                result_text.insert(tk.END, f"• Gian {idx + 1:02d}: {data['name']} ({data['val']} điểm - {data['weight']}kg)\n")
        
        lbl_res.config(text=f"TỔNG GIÁ TRỊ: {total_val}", fg="#27ae60")
    except ValueError:
        messagebox.showerror("Lỗi", "Vui lòng nhập số tải trọng!")

# --- UI Setup ---
root = tk.Tk()
root.title("Hệ thống Quản lý ngày hội việc làm JOBFAIR 2024 - HCMUT(VNU)")
root.geometry("950x750")

notebook = ttk.Notebook(root)
notebook.pack(fill="both", expand=True, padx=10, pady=10)

# TAB 1: SƠ ĐỒ & ĐĂNG KÝ
tab1 = tk.Frame(notebook)
notebook.add(tab1, text="  SƠ ĐỒ & ĐĂNG KÝ  ")

map_frame = tk.LabelFrame(tab1, text=" Sơ đồ vị trí gian hàng (Ấn để Sửa/Xóa) ", padx=10, pady=10)
map_frame.pack(pady=20, padx=20, fill="x")

buttons = []
for i in range(30):
    # Hiển thị i + 1 để người dùng thấy số từ 1 đến 30[cite: 1]
    btn = tk.Button(map_frame, text=str(i + 1), width=6, height=2, font=("Arial", 9), command=lambda idx=i: show_info(idx))
    btn.grid(row=i//15, column=i%15, padx=3, pady=3)
    buttons.append(btn)

reg_frame = tk.LabelFrame(tab1, text=" Đăng ký Doanh nghiệp mới ", padx=20, pady=20)
reg_frame.pack(fill="x", padx=20, pady=10)

tk.Label(reg_frame, text="Tên công ty:").grid(row=0, column=0, sticky="w")
entry_name = tk.Entry(reg_frame, width=30)
entry_name.grid(row=0, column=1, padx=10)

tk.Label(reg_frame, text="Giá trị:").grid(row=0, column=2, sticky="w")
entry_val = tk.Entry(reg_frame, width=8)
entry_val.grid(row=0, column=3, padx=10)

tk.Label(reg_frame, text="Nặng (kg):").grid(row=0, column=4, sticky="w")
entry_weight = tk.Entry(reg_frame, width=8)
entry_weight.grid(row=0, column=5, padx=10)

tk.Label(reg_frame, text="Gian số (1-30):").grid(row=1, column=0, sticky="w", pady=10)
entry_booth = tk.Entry(reg_frame, width=8)
entry_booth.grid(row=1, column=1, sticky="w", padx=10)
tk.Label(reg_frame, text="(Để trống = Tự động)", font=("Arial", 8, "italic")).grid(row=1, column=1, padx=70)

btn_reg = tk.Button(tab1, text="XÁC NHẬN ĐĂNG KÝ", bg="#f39c12", fg="white", font=("Arial", 11, "bold"), width=25, height=2, command=on_register)
btn_reg.pack(pady=10)

# TAB 2: Tối ưu
tab2 = tk.Frame(notebook)
notebook.add(tab2, text="  PHÂN TÍCH TỐI ƯU (KNAPSACK)  ")

knap_input = tk.Frame(tab2, pady=20)
knap_input.pack()
tk.Label(knap_input, text="Giới hạn tải trọng sàn (kg): ", font=("Arial", 11)).pack(side="left")
entry_max_w = tk.Entry(knap_input, font=("Arial", 11), width=12)
entry_max_w.pack(side="left", padx=10)
tk.Button(knap_input, text="CHẠY PHÂN TÍCH", bg="#3498db", fg="white", font=("Arial", 10, "bold"), command=run_knapsack_v2).pack(side="left")

lbl_res = tk.Label(tab2, text="TỔNG GIÁ TRỊ: --", font=("Arial", 15, "bold"), pady=10)
lbl_res.pack()

result_text = tk.Text(tab2, height=20, width=80, font=("Courier New", 10), padx=10, pady=10)
result_text.pack(pady=10)

root.mainloop()