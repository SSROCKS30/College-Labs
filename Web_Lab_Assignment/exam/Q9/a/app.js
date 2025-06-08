const express = require('express');
const path = require('path');
const app = express();
const PORT = 3000;

// Set up static files
app.use(express.static('public'));

// Routes
app.get('/', (req, res) => {
    res.sendFile(__dirname  + '/home.html');
});

app.get('/home', (req, res) => {
    res.sendFile(__dirname  + '/home.html');
});

app.get('/registration', (req, res) => {
    res.sendFile(__dirname  + '/registration.html');
});

app.get('/announcements', (req, res) => {
    res.sendFile(__dirname  + '/announcements.html');
});

app.get('/contact', (req, res) => {
    res.sendFile(__dirname  + '/contact.html');
});

// Start server
app.listen(PORT, () => {
    console.log(`Training site running on http://localhost:${PORT}`);
}); 