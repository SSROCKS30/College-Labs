const express = require('express');
const path = require('path');
const { requestLogger, visitCounter } = require('./middleware');
const app = express();
const PORT = 3000;

// Apply middleware to all routes
app.use(requestLogger);
app.use(visitCounter);

// Set up static files
app.use(express.static('public'));

// Routes
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'views', 'index.html'));
});

// Start server
app.listen(PORT, () => {
    console.log(`Middleware demo server running on http://localhost:${PORT}`);
    console.log('Custom middleware active: Request Logger & Visit Counter');
}); 