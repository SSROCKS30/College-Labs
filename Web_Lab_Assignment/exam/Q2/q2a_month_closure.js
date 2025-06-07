function createMonthConverter() {
    const months = [
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    ];
    
    return function(monthNumber) {
        let num = Math.floor(Number(monthNumber));
        
        if (num < 1 || num > 12) {
            return "Bad Number";
        }
        
        return months[num - 1];
    };
}

const getMonthName = createMonthConverter();

function convertMonth() {
    const input = document.getElementById('monthInput').value;
    const result = getMonthName(input);
    document.getElementById('output').textContent = result;
} 