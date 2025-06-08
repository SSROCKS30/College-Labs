// Q8(a) - Vowel Count Function
// Usage: node q8a_vowel_count.js

function count(text) {
    
    const vowels = {a: 0, e: 0, i: 0, o: 0, u: 0};
    
    for (let char of text.toLowerCase()) {
        if (vowels.hasOwnProperty(char)) {
            vowels[char]++;
        }
    }
    
    let ans = "";
    
    for (let vowel in vowels) {
        ans = ans + ", " + vowel + " - " + vowels[vowel];
    }
    
    return ans;
}

function countVowels(){
    const text = document.getElementById('textInput').value;

    const output = document.getElementById('result');
    ans = count(text);
    output.textContent = ans;
}