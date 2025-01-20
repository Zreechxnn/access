import cv2
import serial
import pytesseract
from difflib import SequenceMatcher
import time

pytesseract.pytesseract.tesseract_cmd = r"C:\Program Files\Tesseract-OCR\tesseract.exe"

try:
    arduino = serial.Serial(port='COM4', baudrate=9600, timeout=1)
    print("Koneksi ke Arduino berhasil.")
except serial.SerialException as e:
    print(f"Error: Tidak dapat terhubung ke Arduino. {e}")
    exit()

cap = cv2.VideoCapture(0)
if not cap.isOpened():
    raise ValueError("Kamera tidak dapat dibuka.")

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

def control_servo(state: bool):
    try:
        if state:
            arduino.write(b'ON\n')
            print("Kamar terbuka (Servo ON)")
        else:
            arduino.write(b'OFF\n')
            print("Kamar tertutup (Servo OFF)")
    except serial.SerialException as e:
        print(f"Error: Gagal mengirim data ke Arduino. {e}")

def play_music(command):
    try:
        if command == "kartini":
            arduino.write(b'MUSIK1\n')
            print("Memainkan ibu kita kartini...")
        elif command == "FUFUFAFAFA":
            arduino.write(b'MUSIK2\n')
            print("Memainkan Fufufafafa...")
    except serial.SerialException as e:
        print(f"Error: Gagal mengirim data ke Arduino. {e}")

def similar(a, b):
    """Menghitung tingkat kemiripan teks menggunakan SequenceMatcher."""
    return SequenceMatcher(None, a.lower(), b.lower()).ratio()

kamar_terbuka = False
last_valid_command = ""

try:
    print("Program dimulai. Kamera aktif...")
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Gagal menangkap frame. Memeriksa ulang kamera...")
            continue

        gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        gray_frame = cv2.GaussianBlur(gray_frame, (5, 5), 0)

        detected_text = pytesseract.image_to_string(gray_frame, config='--psm 6').strip()

        if similar(detected_text, "Buka kamar") >= 0.75:
            if not kamar_terbuka and last_valid_command != "Buka kamar":
                kamar_terbuka = True
                last_valid_command = "Buka kamar"
                control_servo(True)
        
        elif similar(detected_text, "Tutup kamar") >= 0.75:
            if kamar_terbuka and last_valid_command != "Tutup kamar":
                kamar_terbuka = False
                last_valid_command = "Tutup kamar"
                control_servo(False)

        elif similar(detected_text, "kartini") >= 0.75:
            if last_valid_command != "kartini":
                last_valid_command = "kartini"
                play_music("kartini")

        elif similar(detected_text, "Fufufafafa") >= 0.75:
            if last_valid_command != "Fufufafafa":
                last_valid_command = "Fufufafafa"
                play_music("FUFUFAFAFA")

        if kamar_terbuka:
            cv2.putText(frame, "PINTU TERBUKA", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        else:
            cv2.putText(frame, "PINTU TERTUTUP", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

        cv2.imshow("Deteksi Kartu", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("Program dihentikan oleh pengguna.")
            break

except KeyboardInterrupt:
    print("Program dihentikan dengan Ctrl+C.")
finally:
    cap.release()
    cv2.destroyAllWindows()
    control_servo(False)
    arduino.close()
    print("Program keluar.")
