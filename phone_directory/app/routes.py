from flask import Blueprint, render_template, request, redirect, url_for, flash, send_file
from flask_login import login_user, logout_user, login_required, current_user
from app.models import Contact, AdminUser
from app.forms import ContactForm, LoginForm
from app.utils import export_to_excel, import_from_excel
from app import db, login_manager
from datetime import datetime
import os
import uuid
from werkzeug.utils import secure_filename

main_bp = Blueprint('main', __name__)
admin_bp = Blueprint('admin', __name__)

@login_manager.user_loader
def load_user(user_id):
    return AdminUser.query.get(int(user_id))

@main_bp.route('/')
def index():
    contacts = Contact.query.all()
    return render_template('public/directory.html', contacts=contacts)

@admin_bp.route('/login', methods=['GET', 'POST'])
def login():
    if current_user.is_authenticated:
        return redirect(url_for('admin.dashboard'))
    
    form = LoginForm()
    if form.validate_on_submit():
        admin = AdminUser.query.filter_by(username=form.username.data).first()
        if admin and admin.check_password(form.password.data):
            login_user(admin)
            next_page = request.args.get('next')
            return redirect(next_page) if next_page else redirect(url_for('admin.dashboard'))
        else:
            flash('Неверное имя пользователя или пароль')
    
    return render_template('admin/login.html', form=form)

@admin_bp.route('/logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for('main.index'))

@admin_bp.route('/')
@login_required
def dashboard():
    contacts = Contact.query.all()
    return render_template('admin/dashboard.html', contacts=contacts)

@admin_bp.route('/create', methods=['GET', 'POST'])
@login_required
def create():
    form = ContactForm()
    if form.validate_on_submit():
        contact = Contact(
            full_name=form.full_name.data,
            work_phone=form.work_phone.data,
            mobile_phone=form.mobile_phone.data,
            email=form.email.data,
            notes=form.notes.data
        )
        db.session.add(contact)
        db.session.commit()
        flash('Контакт успешно создан')
        return redirect(url_for('admin.dashboard'))
    
    return render_template('admin/create.html', form=form)

@admin_bp.route('/edit/<int:id>', methods=['GET', 'POST'])
@login_required
def edit(id):
    contact = Contact.query.get_or_404(id)
    form = ContactForm(obj=contact)
    
    if form.validate_on_submit():
        contact.full_name = form.full_name.data
        contact.work_phone = form.work_phone.data
        contact.mobile_phone = form.mobile_phone.data
        contact.email = form.email.data
        contact.notes = form.notes.data
        
        db.session.commit()
        flash('Контакт успешно обновлен')
        return redirect(url_for('admin.dashboard'))
    
    return render_template('admin/edit.html', form=form, contact=contact)

@admin_bp.route('/delete/<int:id>')
@login_required
def delete(id):
    contact = Contact.query.get_or_404(id)
    db.session.delete(contact)
    db.session.commit()
    flash('Контакт успешно удален')
    return redirect(url_for('admin.dashboard'))

@admin_bp.route('/export')
@login_required
def export():
    buffer = export_to_excel()
    buffer.seek(0)
    return send_file(
        buffer,
        as_attachment=True,
        download_name=f'phone_directory_{datetime.now().strftime("%Y%m%d_%H%M%S")}.xlsx',
        mimetype='application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'
    )

@admin_bp.route('/import', methods=['POST'])
@login_required
def import_contacts():
    if 'file' not in request.files:
        flash('Файл не выбран')
        return redirect(url_for('admin.dashboard'))
    
    file = request.files['file']
    if file.filename == '':
        flash('Файл не выбран')
        return redirect(url_for('admin.dashboard'))
    
    if file and file.filename.endswith('.xlsx'):
        # Создаем безопасное имя файла
        filename = secure_filename(file.filename)
        name, ext = os.path.splitext(filename)
        safe_filename = f"{name}_{uuid.uuid4().hex}{ext}"
        temp_path = os.path.join('static/uploads', safe_filename)
        
        # Сохраняем временный файл
        file.save(temp_path)
        
        try:
            imported_count = import_from_excel(temp_path)
            flash(f'Успешно импортировано {imported_count} контактов')
        except Exception as e:
            flash(f'Ошибка при импорте: {str(e)}')
        finally:
            # Удаляем временный файл
            if os.path.exists(temp_path):
                os.remove(temp_path)
    else:
        flash('Неверный формат файла. Требуется .xlsx')
    
    return redirect(url_for('admin.dashboard'))