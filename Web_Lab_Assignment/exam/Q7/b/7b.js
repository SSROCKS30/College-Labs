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

  if (!name || !usn || !subject || !grade) {
    return res.send('All fields are required');
  }

  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('examdb');
    const collection = db.collection('exam_students');

    await collection.insertOne({ name, usn, subject, grade });

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
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('examdb');
    const collection = db.collection('exam_students');

    const sGradeStudents = await collection.find({ grade: 'S' }).toArray();

    let html = '<h2>Students with "S" Grade</h2>';
    if (sGradeStudents.length === 0) {
      html += '<p>No students found with "S" grade.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>Name</th><th>USN</th><th>Subject</th><th>Grade</th></tr>';
      sGradeStudents.forEach(student => {
        html += `<tr>
          <td>${student.name}</td>
          <td>${student.usn}</td>
          <td>${student.subject}</td>
          <td style="background-color: #90EE90; font-weight: bold;">${student.grade}</td>
        </tr>`;
      });
      html += '</table>';
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