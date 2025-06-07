function translate(text) {
    const vowels = 'aeiouAEIOU';
    let result = '';
    
    for (let i = 0; i < text.length; i++) {
        let char = text[i];
        
        if (!vowels.includes(char)) {
            result += char + 'o' + char;
        } else {
            result += char;
        }
    }
    
    return result;
}

function translateText() {
    const input = document.getElementById('textInput').value;
    const result = translate(input);
    document.getElementById('output').textContent = result;
} 