const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '2b.html'));
});

app.get('/insert', async (req, res) => {
  const { name, usn, semester, examFee } = req.query;
  const parsedSemester = parseInt(semester);
  const parsedExamFee = parseFloat(examFee);

  if (!name || !usn || isNaN(parsedSemester) || isNaN(parsedExamFee)) {
    return res.send('Invalid input');
  }

  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('examdb');
    const collection = db.collection('students');

    await collection.insertOne({ 
      name, 
      usn, 
      semester: parsedSemester, 
      examFee: parsedExamFee,
      hasPaidFee: parsedExamFee > 0
    });

    res.send(`Student ${name} registered successfully! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/unpaid', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('examdb');
    const collection = db.collection('students');

    const unpaidStudents = await collection.find({ 
      $or: [
        { examFee: 0 },
        { examFee: { $exists: false } },
        { hasPaidFee: false }
      ]
    }).toArray();

    let html = '<h2>Students Who Haven\'t Paid Exam Fees</h2>';
    if (unpaidStudents.length === 0) {
      html += '<p>No unpaid students found.</p>';
    } else {
      html += '<ul>';
      unpaidStudents.forEach(student => {
        html += `<li>Name: ${student.name}, USN: ${student.usn}, Semester: ${student.semester}, Fee: ${student.examFee || 0}</li>`;
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

app.get('/delete-unpaid', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('examdb');
    const collection = db.collection('students');

    const result = await collection.deleteMany({ 
      $or: [
        { examFee: 0 },
        { examFee: { $exists: false } },
        { hasPaidFee: false }
      ]
    });

    res.send(`${result.deletedCount} students who haven't paid fees have been deleted. <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.listen(5002, () => {
  console.log('Q2(b) - Listening on port 5002');
}); 