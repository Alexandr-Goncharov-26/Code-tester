import os
from app import create_app

# Создаем приложение
app = create_app()

if __name__ == '__main__':
    # Получаем порт из переменной окружения или используем 5000 по умолчанию
    port = int(os.environ.get('PORT', 5000))
    
    # Запускаем приложение
    app.run(
        host='0.0.0.0',  # Доступно извне контейнера
        port=port,
        debug=False  # Отключаем режим отладки для продакшена
    )