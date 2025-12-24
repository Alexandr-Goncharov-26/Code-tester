// Общие JavaScript функции для телефонного справочника

// Подтверждение удаления
function confirmDelete(message) {
    return confirm(message || 'Вы уверены, что хотите удалить эту запись?');
}

// Динамический поиск
document.addEventListener('DOMContentLoaded', function() {
    // Добавляем функциональность поиска ко всем таблицам
    const searchInputs = document.querySelectorAll('.search-input');
    searchInputs.forEach(input => {
        input.addEventListener('keyup', function() {
            const searchValue = this.value.toLowerCase();
            const table = this.closest('.table-container').querySelector('table');
            const rows = table.querySelectorAll('tbody tr');
            
            rows.forEach(row => {
                const rowText = row.textContent.toLowerCase();
                row.style.display = rowText.includes(searchValue) ? '' : 'none';
            });
        });
    });
});

// Форматирование телефонных номеров (при необходимости)
function formatPhoneNumber(input) {
    // Удаляем все, кроме цифр
    let value = input.value.replace(/\D/g, '');
    
    // Применяем формат +7 (XXX) XXX-XXXX
    if (value.length >= 11) {
        value = value.substring(0, 11);
    }
    
    // Форматируем
    if (value.length > 0) {
        value = '+7 (' + value.substring(1, 4) + ') ' + 
                value.substring(4, 7) + '-' + 
                value.substring(7, 11);
    }
    
    input.value = value;
}