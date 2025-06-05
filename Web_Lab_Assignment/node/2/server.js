const http = require('http');
const fs = require('fs');
const path = require('path');
const querystring = require('querystring');

const port = 3002;

const server = http.createServer((req, res) => {
  if (req.method === 'GET' && req.url === '/') {
    const filePath = path.join(__dirname, 'form.html');
    fs.readFile(filePath, (err, content) => {
      if (err) {
        res.writeHead(500, { 'Content-Type': 'text/plain' });
        res.end('Error loading form.html');
        return;
      }
      res.writeHead(200, { 'Content-Type': 'text/html' });
      res.end(content);
    });
  } else if (req.method === 'POST' && req.url === '/submit') {
    let body = '';
    req.on('data', chunk => {
      body += chunk.toString();
    });
    req.on('end', () => {
      const formData = querystring.parse(body);
      const { usn, dob, branch } = formData;

      console.log('Received student details:');
      console.log(`USN: ${usn}`);
      console.log(`Date of Birth: ${dob}`);
      console.log(`Branch: ${branch}`);

      res.writeHead(200, { 'Content-Type': 'text/html' });
      res.end(`<h1>Student Details Submitted Successfully!</h1>
                <p>USN: ${usn}</p>
                <p>Date of Birth: ${dob}</p>
                <p>Branch: ${branch}</p>
                <a href="/">Go back to form</a>`);
    });
  } else {
    res.writeHead(404, { 'Content-Type': 'text/plain' });
    res.end('Not Found');
  }
});

server.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
}); 