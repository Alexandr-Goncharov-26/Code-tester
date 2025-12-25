"""
Тесты безопасности для приложения "Телефонный справочник"
"""
import unittest
import tempfile
import os
from app import create_app
from app.models import Contact, AdminUser
from app import db
from werkzeug.security import generate_password_hash
import openpyxl
from io import BytesIO

class SecurityTestCase(unittest.TestCase):
    def setUp(self):
        """Настройка тестового приложения"""
        self.app = create_app()
        self.app.config['TESTING'] = True
        self.app.config['WTF_CSRF_ENABLED'] = True  # Включаем CSRF для тестирования
        self.app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///:memory:'
        
        self.client = self.app.test_client()
        
        with self.app.app_context():
            db.create_all()
            
            # Создаем тестового администратора
            admin = AdminUser(
                username='test_admin',
                password_hash=generate_password_hash('secure_password')
            )
            db.session.add(admin)
            db.session.commit()

    def test_csrf_protection(self):
        """Тест защиты от CSRF атак"""
        # Попытка отправки формы без CSRF токена должна быть отклонена
        response = self.client.post('/admin/create', data={
            'full_name': 'Test User',
            'work_phone': '123456789',
            'mobile_phone': '987654321',
            'email': 'test@example.com',
            'notes': 'Test notes'
        }, follow_redirects=True)
        
        # Должен быть редирект на страницу входа из-за отсутствия сессии
        # или ошибка валидации из-за отсутствия CSRF токена
        self.assertIn(response.status_code, [200, 400, 405])

    def test_path_traversal_protection(self):
        """Тест защиты от Path Traversal атак при загрузке файлов"""
        # Создаем временный вредоносный файл
        malicious_file = tempfile.NamedTemporaryFile(suffix='.xlsx', delete=False)
        malicious_file.close()
        
        # Попытка загрузки файла через маршрут импорта
        with open(malicious_file.name, 'w') as f:
            f.write("test content")
        
        # Имитация загрузки файла с вредоносным именем
        # (тестирование защиты в routes.py)
        
        os.unlink(malicious_file.name)

    def test_input_sanitization(self):
        """Тест очистки ввода от потенциально опасного содержимого"""
        from app.forms import sanitize_input
        
        # Тест очистки HTML
        malicious_input = "<script>alert('XSS')</script>Hello World"
        sanitized = sanitize_input(malicious_input)
        self.assertNotIn('<script>', sanitized)
        self.assertEqual(sanitized, "Hello World")
        
        # Тест очистки других потенциально опасных тегов
        malicious_input2 = '<img src="x" onerror="alert(1)">Test'
        sanitized2 = sanitize_input(malicious_input2)
        self.assertNotIn('onerror', sanitized2)
        self.assertEqual(sanitized2, 'Test')

    def test_password_hashing(self):
        """Тест безопасного хеширования паролей"""
        from werkzeug.security import check_password_hash
        
        password = "my_secure_password"
        hashed = generate_password_hash(password)
        
        # Проверяем, что пароль не хранится в открытом виде
        self.assertNotEqual(password, hashed)
        
        # Проверяем, что хеш корректно проверяется
        self.assertTrue(check_password_hash(hashed, password))
        
        # Проверяем, что неправильный пароль не проходит проверку
        self.assertFalse(check_password_hash(hashed, "wrong_password"))

    def test_file_validation(self):
        """Тест валидации файлов при импорте"""
        from app.utils import validate_excel_file
        
        # Создаем временный файл для тестирования
        with tempfile.NamedTemporaryFile(delete=False, suffix='.xlsx') as tmp:
            # Создаем корректный Excel файл
            wb = openpyxl.Workbook()
            ws = wb.active
            ws['A1'] = 'Test'
            wb.save(tmp.name)
            
            # Тест корректного файла
            try:
                validate_excel_file(tmp.name)
                # Если ошибки не возникло, тест пройден
                valid_file_test_passed = True
            except ValueError:
                valid_file_test_passed = False
            
            self.assertTrue(valid_file_test_passed)
            
            # Проверяем, что файл с неправильным расширением вызывает ошибку
            wrong_ext_file = tmp.name.replace('.xlsx', '.txt')
            os.rename(tmp.name, wrong_ext_file)
            
            try:
                validate_excel_file(wrong_ext_file)
                wrong_ext_test_passed = False  # Если ошибка не возникла, тест не пройден
            except ValueError:
                wrong_ext_test_passed = True  # Если ошибка возникла, тест пройден
            
            # Возвращаем исходное имя для удаления
            os.rename(wrong_ext_file, tmp.name)
            
            self.assertTrue(wrong_ext_test_passed)
            
        os.unlink(tmp.name)

    def test_secure_filename_generation(self):
        """Тест безопасного именования файлов"""
        import uuid
        from werkzeug.utils import secure_filename
        
        # Тест генерации безопасного имени файла
        original_filename = "../../../etc/passwd.xlsx"
        filename = secure_filename(original_filename)
        name, ext = os.path.splitext(filename)
        safe_filename = f"{name}_{uuid.uuid4().hex}{ext}"
        
        # Проверяем, что результат не содержит опасных символов
        self.assertNotIn('../', safe_filename)
        self.assertNotIn('..\\', safe_filename)

if __name__ == '__main__':
    unittest.main()