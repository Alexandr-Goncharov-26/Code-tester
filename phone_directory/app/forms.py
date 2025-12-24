from flask_wtf import FlaskForm
from wtforms import StringField, TextAreaField, PasswordField, SubmitField
from wtforms.validators import DataRequired, Optional, Email, Length

class ContactForm(FlaskForm):
    full_name = StringField('ФИО', validators=[DataRequired(), Length(max=100)])
    work_phone = StringField('Телефон рабочий', validators=[Optional(), Length(max=20)])
    mobile_phone = StringField('Телефон сотовый', validators=[Optional(), Length(max=20)])
    email = StringField('Электронная почта', validators=[Optional(), Email(), Length(max=100)])
    notes = TextAreaField('Примечания', validators=[Optional()])
    submit = SubmitField('Сохранить')

class LoginForm(FlaskForm):
    username = StringField('Имя пользователя', validators=[DataRequired()])
    password = PasswordField('Пароль', validators=[DataRequired()])
    submit = SubmitField('Войти')