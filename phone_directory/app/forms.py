from flask_wtf import FlaskForm
from wtforms import StringField, TextAreaField, PasswordField, SubmitField
from wtforms.validators import DataRequired, Optional, Email, Length
import bleach

def sanitize_input(text):
    """Очистка ввода от потенциально опасного HTML"""
    if text:
        # Очищаем текст от потенциально опасных тегов и атрибутов
        return bleach.clean(text, tags=[], attributes={}, strip=True)
    return text

class ContactForm(FlaskForm):
    full_name = StringField('ФИО', validators=[DataRequired(), Length(max=100)])
    work_phone = StringField('Телефон рабочий', validators=[Optional(), Length(max=20)])
    mobile_phone = StringField('Телефон сотовый', validators=[Optional(), Length(max=20)])
    email = StringField('Электронная почта', validators=[Optional(), Email(), Length(max=100)])
    notes = TextAreaField('Примечания', validators=[Optional()])
    submit = SubmitField('Сохранить')
    
    def validate(self):
        # Вызываем стандартную валидацию
        rv = FlaskForm.validate(self)
        if not rv:
            return False
        
        # Очищаем потенциально опасные данные
        if self.full_name.data:
            self.full_name.data = sanitize_input(self.full_name.data)
        if self.work_phone.data:
            self.work_phone.data = sanitize_input(self.work_phone.data)
        if self.mobile_phone.data:
            self.mobile_phone.data = sanitize_input(self.mobile_phone.data)
        if self.email.data:
            self.email.data = sanitize_input(self.email.data)
        if self.notes.data:
            self.notes.data = sanitize_input(self.notes.data)
        
        return True

class LoginForm(FlaskForm):
    username = StringField('Имя пользователя', validators=[DataRequired()])
    password = PasswordField('Пароль', validators=[DataRequired()])
    submit = SubmitField('Войти')