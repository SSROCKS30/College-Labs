const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '1b.html'));
});

app.get('/insert', async (req, res) => {
  const { name, usn, scode, marks } = req.query;
  const parsedMarks = parseInt(marks);

  let client;
  try {
    // Connect to MongoDB
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('mydb');
    const collection = db.collection('student');

    // Insert a document
    await collection.insertOne({ usn, name, scode, marks: parsedMarks });

    // Find students with marks < 20
    const lowScorers = await collection.find({ marks: { $lt: 20 } }).toArray();

    let html = `<h1>Student ${name} marks (${parsedMarks}) for ${scode} added successfully!</h1>`;
    html += '<h2>Students with marks < 20</h2>';
    if (lowScorers.length === 0) {
      html += '<p>No students found with marks < 20.</p>';
    } else {
      html += '<ul>';
      lowScorers.forEach(student => {
        html += `<li> Name: ${student.name} | USN: ${student.usn} | Subject: ${student.scode} | Marks: ${student.marks} </li>`;
      });
      html += '</ul>';
    }
    html += '<br><a href="/">Go Back</a>';
    
    console.log('Students with marks < 20:', lowScorers);
    res.send(html);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.listen(5000, () => {
  console.log('Listening on port 5000');
});