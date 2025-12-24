import openpyxl
from io import BytesIO
from datetime import datetime
from app.models import Contact
from app import db

def export_to_excel():
    """Экспорт контактов в Excel файл"""
    # Создаем новую рабочую книгу
    wb = openpyxl.Workbook()
    ws = wb.active
    ws.title = "Телефонный справочник"
    
    # Заголовки
    headers = ['ФИО', 'Телефон рабочий', 'Телефон сотовый', 'Электронная почта', 'Примечания']
    for col, header in enumerate(headers, 1):
        ws.cell(row=1, column=col, value=header)
    
    # Данные
    contacts = Contact.query.all()
    for row, contact in enumerate(contacts, 2):
        ws.cell(row=row, column=1, value=contact.full_name)
        ws.cell(row=row, column=2, value=contact.work_phone)
        ws.cell(row=row, column=3, value=contact.mobile_phone)
        ws.cell(row=row, column=4, value=contact.email)
        ws.cell(row=row, column=5, value=contact.notes)
    
    # Сохраняем в буфер
    buffer = BytesIO()
    wb.save(buffer)
    buffer.seek(0)
    
    return buffer

def import_from_excel(file_path):
    """Импорт контактов из Excel файла"""
    wb = openpyxl.load_workbook(file_path)
    ws = wb.active
    
    # Пропускаем заголовки
    rows = list(ws.iter_rows(min_row=2, values_only=True))
    
    imported_count = 0
    for row in rows:
        if row[0]:  # Проверяем, что ФИО не пустое
            contact = Contact(
                full_name=row[0],
                work_phone=row[1],
                mobile_phone=row[2],
                email=row[3],
                notes=row[4]
            )
            db.session.add(contact)
            imported_count += 1
    
    db.session.commit()
    return imported_count