const express = require('express');
const app = express();
const path = require('path');

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '6b.html'));
});

app.get('/cse', (req, res) => {
  const html = `
    <!DOCTYPE html>
    <html>
    <head>
      <title>Computer Science Engineering</title>
      <style>
        body { 
          font-family: 'Courier New', monospace; 
          margin: 40px; 
          background-color: #1a1a2e;
          color: #00ff00;
        }
        nav { background-color: #16213e; padding: 15px; text-align: center; border: 2px solid #00ff00; }
        nav a { color: #00ff00; text-decoration: none; margin: 0 20px; padding: 10px 15px; border-radius: 5px; }
        nav a:hover { background-color: #0e3460; }
        .content { margin-top: 30px; }
        h1 { color: #00ff00; text-shadow: 0 0 10px #00ff00; font-size: 2.5em; }
        .info-box { border: 2px solid #00ff00; padding: 20px; margin: 20px 0; background-color: #0f0f23; }
      </style>
    </head>
    <body>
      <nav>
        <a href="/">Home</a>
        <a href="/cse">Computer Science</a>
        <a href="/ece">Electronics & Communication</a>
        <a href="/me">Mechanical Engineering</a>
        <a href="/civil">Civil Engineering</a>
      </nav>
      
      <div class="content">
        <h1>Computer Science Engineering</h1>
        
        <div class="info-box">
          <h2>About CSE</h2>
          <p>Computer Science Engineering deals with design, implementation, and management of information systems and computer hardware & software.</p>
        </div>
        
        <div class="info-box">
          <h2>Key Subjects</h2>
          <ul>
            <li>Data Structures & Algorithms</li>
            <li>Operating Systems</li>
            <li>Database Management Systems</li>
            <li>Computer Networks</li>
            <li>Software Engineering</li>
            <li>Machine Learning</li>
          </ul>
        </div>
        
        <div class="info-box">
          <h2>Career Opportunities</h2>
          <p>Software Developer, Data Scientist, Cybersecurity Analyst, AI Engineer, Web Developer, Mobile App Developer</p>
        </div>
      </div>
    </body>
    </html>
  `;
  res.send(html);
});

app.get('/ece', (req, res) => {
  const html = `
    <!DOCTYPE html>
    <html>
    <head>
      <title>Electronics & Communication Engineering</title>
      <style>
        body { 
          font-family: 'Georgia', serif; 
          margin: 40px; 
          background-color: #2c0e37;
          color: #ff6b35;
        }
        nav { background-color: #4a1556; padding: 15px; text-align: center; border: 2px solid #ff6b35; }
        nav a { color: #ff6b35; text-decoration: none; margin: 0 20px; padding: 10px 15px; border-radius: 5px; }
        nav a:hover { background-color: #5e1a6b; }
        .content { margin-top: 30px; }
        h1 { color: #ff6b35; text-shadow: 0 0 10px #ff6b35; font-size: 2.5em; }
        .info-box { border: 2px solid #ff6b35; padding: 20px; margin: 20px 0; background-color: #3d1049; }
      </style>
    </head>
    <body>
      <nav>
        <a href="/">Home</a>
        <a href="/cse">Computer Science</a>
        <a href="/ece">Electronics & Communication</a>
        <a href="/me">Mechanical Engineering</a>
        <a href="/civil">Civil Engineering</a>
      </nav>
      
      <div class="content">
        <h1>Electronics & Communication Engineering</h1>
        
        <div class="info-box">
          <h2>About ECE</h2>
          <p>Electronics & Communication Engineering deals with electronic devices, circuits, communication equipment, and related technologies.</p>
        </div>
        
        <div class="info-box">
          <h2>Key Subjects</h2>
          <ul>
            <li>Analog & Digital Electronics</li>
            <li>Signal Processing</li>
            <li>Communication Systems</li>
            <li>Microprocessors</li>
            <li>VLSI Design</li>
            <li>Embedded Systems</li>
          </ul>
        </div>
        
        <div class="info-box">
          <h2>Career Opportunities</h2>
          <p>Electronics Engineer, Telecommunications Engineer, VLSI Designer, Embedded Systems Developer, Signal Processing Engineer</p>
        </div>
      </div>
    </body>
    </html>
  `;
  res.send(html);
});

app.get('/me', (req, res) => {
  const html = `
    <!DOCTYPE html>
    <html>
    <head>
      <title>Mechanical Engineering</title>
      <style>
        body { 
          font-family: 'Times New Roman', serif; 
          margin: 40px; 
          background-color: #1e3a1e;
          color: #ffd700;
        }
        nav { background-color: #2d5a2d; padding: 15px; text-align: center; border: 2px solid #ffd700; }
        nav a { color: #ffd700; text-decoration: none; margin: 0 20px; padding: 10px 15px; border-radius: 5px; }
        nav a:hover { background-color: #3d6a3d; }
        .content { margin-top: 30px; }
        h1 { color: #ffd700; text-shadow: 0 0 10px #ffd700; font-size: 2.5em; }
        .info-box { border: 2px solid #ffd700; padding: 20px; margin: 20px 0; background-color: #264a26; }
      </style>
    </head>
    <body>
      <nav>
        <a href="/">Home</a>
        <a href="/cse">Computer Science</a>
        <a href="/ece">Electronics & Communication</a>
        <a href="/me">Mechanical Engineering</a>
        <a href="/civil">Civil Engineering</a>
      </nav>
      
      <div class="content">
        <h1>Mechanical Engineering</h1>
        
        <div class="info-box">
          <h2>About ME</h2>
          <p>Mechanical Engineering involves design, manufacturing, and maintenance of mechanical systems, machines, and thermal devices.</p>
        </div>
        
        <div class="info-box">
          <h2>Key Subjects</h2>
          <ul>
            <li>Thermodynamics</li>
            <li>Fluid Mechanics</li>
            <li>Machine Design</li>
            <li>Manufacturing Processes</li>
            <li>Heat Transfer</li>
            <li>Automotive Engineering</li>
          </ul>
        </div>
        
        <div class="info-box">
          <h2>Career Opportunities</h2>
          <p>Mechanical Designer, Manufacturing Engineer, Automotive Engineer, HVAC Engineer, Project Manager, Quality Control Engineer</p>
        </div>
      </div>
    </body>
    </html>
  `;
  res.send(html);
});

app.get('/civil', (req, res) => {
  const html = `
    <!DOCTYPE html>
    <html>
    <head>
      <title>Civil Engineering</title>
      <style>
        body { 
          font-family: 'Trebuchet MS', sans-serif; 
          margin: 40px; 
          background-color: #1a1a2e;
          color: #e74c3c;
        }
        nav { background-color: #16213e; padding: 15px; text-align: center; border: 2px solid #e74c3c; }
        nav a { color: #e74c3c; text-decoration: none; margin: 0 20px; padding: 10px 15px; border-radius: 5px; }
        nav a:hover { background-color: #2c3e50; }
        .content { margin-top: 30px; }
        h1 { color: #e74c3c; text-shadow: 0 0 10px #e74c3c; font-size: 2.5em; }
        .info-box { border: 2px solid #e74c3c; padding: 20px; margin: 20px 0; background-color: #0f0f23; }
      </style>
    </head>
    <body>
      <nav>
        <a href="/">Home</a>
        <a href="/cse">Computer Science</a>
        <a href="/ece">Electronics & Communication</a>
        <a href="/me">Mechanical Engineering</a>
        <a href="/civil">Civil Engineering</a>
      </nav>
      
      <div class="content">
        <h1>Civil Engineering</h1>
        
        <div class="info-box">
          <h2>About Civil Engineering</h2>
          <p>Civil Engineering deals with design, construction, and maintenance of infrastructure like roads, bridges, buildings, and water supply systems.</p>
        </div>
        
        <div class="info-box">
          <h2>Key Subjects</h2>
          <ul>
            <li>Structural Engineering</li>
            <li>Geotechnical Engineering</li>
            <li>Transportation Engineering</li>
            <li>Environmental Engineering</li>
            <li>Construction Management</li>
            <li>Surveying</li>
          </ul>
        </div>
        
        <div class="info-box">
          <h2>Career Opportunities</h2>
          <p>Structural Engineer, Construction Manager, Urban Planner, Highway Engineer, Water Resources Engineer, Building Inspector</p>
        </div>
      </div>
    </body>
    </html>
  `;
  res.send(html);
});

app.listen(5006, () => {
  console.log('Q6(b) - Listening on port 5006');
}); 