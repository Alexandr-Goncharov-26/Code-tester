from app import create_app, db
from app.models import Contact

app = create_app()

with app.app_context():
    # Проверяем, есть ли уже какие-то контакты
    if Contact.query.count() == 0:
        # Добавляем несколько тестовых контактов
        sample_contacts = [
            Contact(
                full_name="Иванов Иван Иванович",
                work_phone="+7 (495) 123-45-67",
                mobile_phone="+7 (925) 123-45-67",
                email="ivanov@example.com",
                notes="Генеральный директор"
            ),
            Contact(
                full_name="Петров Петр Петрович",
                work_phone="+7 (495) 234-56-78",
                mobile_phone="+7 (926) 234-56-78",
                email="petrov@example.com",
                notes="Главный бухгалтер"
            ),
            Contact(
                full_name="Сидоров Алексей Николаевич",
                work_phone="+7 (495) 345-67-89",
                mobile_phone="+7 (927) 345-67-89",
                email="sidorov@example.com",
                notes="Менеджер по продажам"
            )
        ]
        
        for contact in sample_contacts:
            db.session.add(contact)
        
        db.session.commit()
        print(f"Добавлено {len(sample_contacts)} тестовых контактов")
    else:
        print(f"В базе данных уже есть {Contact.query.count()} контактов")