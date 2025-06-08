const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '7b.html'));
});

app.get('/insert', async (req, res) => {
  const { name, usn, subject, grade } = req.query;

  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('examdb');
    const collection = db.collection('exam_students');

    await collection.insertOne({ name: name, usn: usn, subject: subject, grade: grade });

    res.send(`Student ${name} added successfully with grade ${grade}! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/s-grade', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('examdb');
    const collection = db.collection('exam_students');

    const sGradeStudents = await collection.find({ grade: 'S' }).toArray();

    let html = '<h2>Students with "S" Grade</h2>';
    if (sGradeStudents.length === 0) {
      html += '<p>No students found with "S" grade.</p>';
    } else {
      html += '<ul>';
      sGradeStudents.forEach(student => {
        html += `<li> Name: ${student.name} | USN: ${student.usn} | Subject: ${student.subject} | Grade: ${student.grade} </li>`;
      });
      html += '</ul>';
    }
    html += '<br><a href="/">Go Back</a>';
    
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

app.listen(5007, () => {
  console.log('Q7(b) - Listening on port 5007');
}); 