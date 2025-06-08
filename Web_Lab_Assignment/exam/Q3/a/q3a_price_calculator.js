function createPriceCalculator(taxRate) {
    return function(price) {
        const tax = price * (taxRate / 100);
        const netPrice = price + tax;
        
        return {
            originalPrice: price,
            tax: tax,
            netPrice: netPrice
        };
    };
}

function calculatePrice() {
    const price = parseFloat(document.getElementById('priceInput').value);
    const taxRate = parseFloat(document.getElementById('taxInput').value);
    
    if (isNaN(price) || isNaN(taxRate) || price < 0 || taxRate < 0) {
        alert('Please enter valid positive numbers for both price and tax rate.');
        return;
    }
    
    const calculator = createPriceCalculator(taxRate);
    const result = calculator(price);
    
    document.getElementById('originalPrice').textContent = result.originalPrice.toFixed(2);
    document.getElementById('taxAmount').textContent = result.tax.toFixed(2);
    document.getElementById('netPrice').textContent = result.netPrice.toFixed(2);
} 