# Структура проекта телефонного справочника

```
phone_directory/
├── app/
│   ├── __init__.py
│   ├── models.py
│   ├── views.py
│   ├── forms.py
│   ├── routes.py
│   └── utils.py
├── config/
│   ├── __init__.py
│   └── config.py
├── static/
│   ├── css/
│   │   └── style.css
│   ├── js/
│   │   └── main.js
│   └── uploads/
├── templates/
│   ├── base.html
│   ├── index.html
│   ├── admin/
│   │   ├── login.html
│   │   ├── dashboard.html
│   │   ├── create.html
│   │   ├── edit.html
│   │   └── delete.html
│   └── public/
│       └── directory.html
├── migrations/
├── tests/
├── requirements.txt
├── run.py
└── README.md
```

## Описание компонентов

### app/
- `__init__.py` - инициализация Flask-приложения
- `models.py` - определение моделей базы данных (Контакт, Администратор)
- `views.py` - логика представлений
- `forms.py` - формы для валидации данных
- `routes.py` - маршруты приложения
- `utils.py` - вспомогательные функции (импорт/экспорт Excel, аутентификация)

### config/
- `config.py` - настройки приложения (база данных, секретный ключ и т.д.)

### static/
- `css/` - стили
- `js/` - JavaScript
- `uploads/` - временные файлы для импорта/экспорта

### templates/
- `base.html` - базовый шаблон
- `index.html` - главная страница
- `admin/` - шаблоны для администратора
- `public/` - шаблоны для публичного доступа

### Основные файлы
- `requirements.txt` - зависимости
- `run.py` - точка запуска приложения
- `README.md` - документация