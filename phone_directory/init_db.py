from app import create_app, db
from app.models import AdminUser
from config.config import Config

def init_database():
    app = create_app()
    
    with app.app_context():
        # Создаем все таблицы
        db.create_all()
        
        # Проверяем, существует ли уже администратор
        admin = AdminUser.query.filter_by(username=Config.ADMIN_USERNAME).first()
        if not admin:
            admin = AdminUser(username=Config.ADMIN_USERNAME)
            admin.set_password(Config.ADMIN_PASSWORD)
            db.session.add(admin)
            db.session.commit()
            print(f"Администратор '{Config.ADMIN_USERNAME}' создан")
        else:
            print(f"Администратор '{Config.ADMIN_USERNAME}' уже существует")
        
        print("Инициализация базы данных завершена")

if __name__ == '__main__':
    init_database()