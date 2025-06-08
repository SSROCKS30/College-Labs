const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '12b.html'));
});

app.get('/insert', async (req, res) => {
  const { name, usn, subject, marks } = req.query;
  const parsedMarks = parseInt(marks);

  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('exammarksdb');
    const collection = db.collection('student_marks');

    await collection.insertOne({ name: name, usn: usn, subject: subject, marks: parsedMarks });

    res.send(`<h1>Student ${name} marks (${parsedMarks}) for ${subject} added successfully! </h1><br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/low-marks', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('exammarksdb');
    const collection = db.collection('student_marks');

    const lowMarksStudents = await collection.find({ 
      marks: { $lt: 20 } 
    }).toArray();

    let html = '<h2>Students with Marks Less Than 20</h2>';
    if (lowMarksStudents.length === 0) {
      html += '<p>No students found with marks less than 20.</p>';
    } else {
      html += '<ul>';
      lowMarksStudents.forEach(student => {
        html += `<li> Name: ${student.name} | USN: ${student.usn} | Subject: ${student.subject} | Marks: ${student.marks} </li>`;
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

app.get('/all-marks', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('exammarksdb');
    const collection = db.collection('student_marks');

    const allStudents = await collection.find({}).toArray();

    let html = '<h2>All Student Marks</h2>';
    if (allStudents.length === 0) {
      html += '<p>No student marks found.</p>';
    } else {
      html += '<ul>';
      allStudents.forEach(student => {
        html += `<li> Name: ${student.name} | USN: ${student.usn} | Subject: ${student.subject} | Marks: ${student.marks} </li>`;
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

app.listen(5012, () => {
  console.log('Q12(b) - Listening on port 5012');
}); 