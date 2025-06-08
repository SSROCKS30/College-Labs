const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '5b.html'));
});

app.get('/insert', async (req, res) => {
  const { name, usn, dept, grade } = req.query;

  if (!name || !usn || !dept || !grade) {
    return res.send('All fields are required');
  }

  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('studentdb');
    const collection = db.collection('students');

    await collection.insertOne({ name, usn, dept, grade });

    res.send(`Student ${name} added successfully! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/update-grade', async (req, res) => {
  const { name, grade } = req.query;

  if (!name || !grade) {
    return res.send('Name and grade are required');
  }

  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('studentdb');
    const collection = db.collection('students');

    const result = await collection.updateOne(
      { name: name },
      { $set: { grade: grade } }
    );

    if (result.matchedCount > 0) {
      res.send(`Grade updated successfully for ${name}! New grade: ${grade} <br><a href="/">Go Back</a>`);
    } else {
      res.send(`No student found with name: ${name} <br><a href="/">Go Back</a>`);
    }

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/display', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('studentdb');
    const collection = db.collection('students');

    const students = await collection.find({}).toArray();

    let html = '<h2>All Students</h2>';
    if (students.length === 0) {
      html += '<p>No students found.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>Name</th><th>USN</th><th>Department</th><th>Grade</th></tr>';
      students.forEach(student => {
        html += `<tr>
          <td>${student.name}</td>
          <td>${student.usn}</td>
          <td>${student.dept}</td>
          <td>${student.grade}</td>
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

app.listen(5005, () => {
  console.log('Q5(b) - Listening on port 5005');
}); 