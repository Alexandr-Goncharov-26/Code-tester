from app import create_app, db

app = create_app()

with app.app_context():
    inspector = db.inspect(db.engine)
    tables = inspector.get_table_names()
    print("Таблицы в базе данных:", tables)
    
    for table in tables:
        columns = inspector.get_columns(table)
        print(f"\nСтолбцы в таблице '{table}':")
        for col in columns:
            print(f"  - {col['name']} ({col['type']})")