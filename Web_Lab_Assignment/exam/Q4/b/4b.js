const express = require('express');
const app = express();
const path = require('path');

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '4b.html'));
});

app.get('/registration', (req, res) => {
  const html = `
    <!DOCTYPE html>
    <html>
    <head>
      <title>Online Training Site - Registration</title>
      <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        nav { background-color: #333; padding: 10px; }
        nav a { color: white; text-decoration: none; margin: 0 15px; }
        nav a:hover { background-color: #555; padding: 5px; }
        .content { margin-top: 20px; }
        form { max-width: 400px; }
        input, select { width: 100%; padding: 8px; margin: 5px 0; }
      </style>
    </head>
    <body>
      <nav>
        <a href="/">Home</a>
        <a href="/registration">Registration</a>
        <a href="/announcements">Announcements</a>
        <a href="/contact">Contact</a>
      </nav>
      
      <div class="content">
        <h1>Course Registration</h1>
        <form>
          <label>Full Name:</label>
          <input type="text" name="name" required><br>
          
          <label>Email:</label>
          <input type="email" name="email" required><br>
          
          <label>Phone:</label>
          <input type="tel" name="phone" required><br>
          
          <label>Course:</label>
          <select name="course" required>
            <option value="">Select Course</option>
            <option value="web-dev">Web Development</option>
            <option value="data-science">Data Science</option>
            <option value="mobile-dev">Mobile Development</option>
            <option value="ai-ml">AI & Machine Learning</option>
          </select><br>
          
          <input type="submit" value="Register">
        </form>
      </div>
    </body>
    </html>
  `;
  res.send(html);
});

app.get('/announcements', (req, res) => {
  const html = `
    <!DOCTYPE html>
    <html>
    <head>
      <title>Online Training Site - Announcements</title>
      <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        nav { background-color: #333; padding: 10px; }
        nav a { color: white; text-decoration: none; margin: 0 15px; }
        nav a:hover { background-color: #555; padding: 5px; }
        .content { margin-top: 20px; }
        .announcement { border: 1px solid #ddd; padding: 15px; margin: 10px 0; }
        .date { color: #666; font-size: 0.9em; }
      </style>
    </head>
    <body>
      <nav>
        <a href="/">Home</a>
        <a href="/registration">Registration</a>
        <a href="/announcements">Announcements</a>
        <a href="/contact">Contact</a>
      </nav>
      
      <div class="content">
        <h1>Announcements</h1>
        
        <div class="announcement">
          <h3>New Course Launch: AI & Machine Learning</h3>
          <p class="date">Posted: December 15, 2024</p>
          <p>We're excited to announce our new comprehensive AI & Machine Learning course starting January 2025.</p>
        </div>
        
        <div class="announcement">
          <h3>Holiday Schedule</h3>
          <p class="date">Posted: December 10, 2024</p>
          <p>Please note that our support team will have limited availability during the holiday season (Dec 24 - Jan 2).</p>
        </div>
        
        <div class="announcement">
          <h3>Platform Maintenance</h3>
          <p class="date">Posted: December 5, 2024</p>
          <p>Scheduled maintenance on December 20th from 2:00 AM to 4:00 AM EST. Some features may be temporarily unavailable.</p>
        </div>
      </div>
    </body>
    </html>
  `;
  res.send(html);
});

app.get('/contact', (req, res) => {
  const html = `
    <!DOCTYPE html>
    <html>
    <head>
      <title>Online Training Site - Contact</title>
      <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        nav { background-color: #333; padding: 10px; }
        nav a { color: white; text-decoration: none; margin: 0 15px; }
        nav a:hover { background-color: #555; padding: 5px; }
        .content { margin-top: 20px; }
        .contact-info { display: flex; flex-wrap: wrap; gap: 30px; }
        .contact-card { border: 1px solid #ddd; padding: 20px; max-width: 300px; }
      </style>
    </head>
    <body>
      <nav>
        <a href="/">Home</a>
        <a href="/registration">Registration</a>
        <a href="/announcements">Announcements</a>
        <a href="/contact">Contact</a>
      </nav>
      
      <div class="content">
        <h1>Contact Us</h1>
        
        <div class="contact-info">
          <div class="contact-card">
            <h3>General Inquiries</h3>
            <p><strong>Email:</strong> info@onlinetraining.com</p>
            <p><strong>Phone:</strong> +1 (555) 123-4567</p>
            <p><strong>Hours:</strong> Mon-Fri 9:00 AM - 6:00 PM EST</p>
          </div>
          
          <div class="contact-card">
            <h3>Technical Support</h3>
            <p><strong>Email:</strong> support@onlinetraining.com</p>
            <p><strong>Phone:</strong> +1 (555) 987-6543</p>
            <p><strong>Hours:</strong> 24/7 Support Available</p>
          </div>
          
          <div class="contact-card">
            <h3>Admissions</h3>
            <p><strong>Email:</strong> admissions@onlinetraining.com</p>
            <p><strong>Phone:</strong> +1 (555) 456-7890</p>
            <p><strong>Hours:</strong> Mon-Sat 8:00 AM - 8:00 PM EST</p>
          </div>
        </div>
        
        <h2>Address</h2>
        <p>Online Training Institute<br>
        123 Education Street<br>
        Learning City, LC 12345<br>
        United States</p>
      </div>
    </body>
    </html>
  `;
  res.send(html);
});

app.listen(5004, () => {
  console.log('Q4(b) - Listening on port 5004');
}); 