const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '9b.html'));
});

app.get('/insert', async (req, res) => {
  const { name, branch, semester } = req.query;

  if (!name || !branch || !semester) {
    return res.send('All fields are required');
  }

  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('studentbranchdb');
    const collection = db.collection('students');

    await collection.insertOne({ 
      name, 
      branch, 
      semester: parseInt(semester) 
    });

    res.send(`Student ${name} from ${branch} branch, ${semester} semester added successfully! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/cse-6th-sem', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('studentbranchdb');
    const collection = db.collection('students');

    const cse6thStudents = await collection.find({ 
      branch: 'CSE', 
      semester: 6 
    }).toArray();

    let html = '<h2>CSE 6th Semester Students</h2>';
    if (cse6thStudents.length === 0) {
      html += '<p>No CSE 6th semester students found.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>Name</th><th>Branch</th><th>Semester</th></tr>';
      cse6thStudents.forEach(student => {
        html += `<tr>
          <td>${student.name}</td>
          <td style="background-color: #90EE90; font-weight: bold;">${student.branch}</td>
          <td style="background-color: #FFE4B5; font-weight: bold;">${student.semester}</td>
        </tr>`;
      });
      html += '</table>';
      html += `<p><strong>Total CSE 6th semester students: ${cse6thStudents.length}</strong></p>`;
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

app.get('/all-students', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('studentbranchdb');
    const collection = db.collection('students');

    const allStudents = await collection.find({}).toArray();

    let html = '<h2>All Students</h2>';
    if (allStudents.length === 0) {
      html += '<p>No students found.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>Name</th><th>Branch</th><th>Semester</th></tr>';
      allStudents.forEach(student => {
        html += `<tr>
          <td>${student.name}</td>
          <td>${student.branch}</td>
          <td>${student.semester}</td>
        </tr>`;
      });
      html += '</table>';
      html += `<p><strong>Total students: ${allStudents.length}</strong></p>`;
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

app.listen(5009, () => {
  console.log('Q9(b) - Listening on port 5009');
}); 