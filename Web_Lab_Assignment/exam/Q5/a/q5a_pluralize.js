function pluralize(noun, number) {
    const unchanging = ['sheep', 'deer', 'fish'];
    
    const irregulars = {
        'goose': 'geese',
        'man': 'men',
        'woman': 'women',
        'child': 'children',
        'foot': 'feet',
        'tooth': 'teeth',
        'mouse': 'mice'
    };
    
    if (number === 1) {
        return `${number} ${noun}`;
    }
    
    if (unchanging.includes(noun.toLowerCase())) {
        return `${number} ${noun}`;
    }
    
    if (irregulars[noun.toLowerCase()]) {
        return `${number} ${irregulars[noun.toLowerCase()]}`;
    }
    
    let plural = noun;
    
    if (/[sxz]$/.test(noun) || /[sh]$/.test(noun) || /ch$/.test(noun)) {
        plural = noun + 'es';
    }
    else if (/[^aeiou]y$/.test(noun)) {
        plural = noun.slice(0, -1) + 'ies';
    }
    else if (/f$/.test(noun)) {
        plural = noun.slice(0, -1) + 'ves';
    }
    else if (/fe$/.test(noun)) {
        plural = noun.slice(0, -2) + 'ves';
    }
    else {
        plural = noun + 's';
    }
    
    return `${number} ${plural}`;
}

function pluralizeWord() {
    const noun = document.getElementById('nounInput').value.trim();
    const number = parseInt(document.getElementById('numberInput').value);
    
    if (!noun) {
        alert('Please enter a noun.');
        return;
    }
    
    if (isNaN(number) || number < 0) {
        alert('Please enter a valid positive number.');
        return;
    }
    
    const result = pluralize(noun, number);
    document.getElementById('output').textContent = result;
} 