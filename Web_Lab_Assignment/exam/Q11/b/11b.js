const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '11b.html'));
});

app.get('/insert', async (req, res) => {
  const { name, usn, branch, totalClasses, attendedClasses } = req.query;
  const parsedTotalClasses = parseInt(totalClasses);
  const parsedAttendedClasses = parseInt(attendedClasses);

  if (!name || !usn || !branch || isNaN(parsedTotalClasses) || isNaN(parsedAttendedClasses)) {
    return res.send('All fields are required and must be valid numbers');
  }

  if (parsedAttendedClasses > parsedTotalClasses) {
    return res.send('Attended classes cannot be more than total classes');
  }

  const attendancePercentage = (parsedAttendedClasses / parsedTotalClasses) * 100;

  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('attendancedb');
    const collection = db.collection('students');

    await collection.insertOne({ 
      name, 
      usn, 
      branch, 
      totalClasses: parsedTotalClasses,
      attendedClasses: parsedAttendedClasses,
      attendancePercentage: Math.round(attendancePercentage * 100) / 100
    });

    res.send(`Student ${name} added successfully with ${attendancePercentage.toFixed(2)}% attendance! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/low-attendance', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('attendancedb');
    const collection = db.collection('students');

    const lowAttendanceStudents = await collection.find({ 
      attendancePercentage: { $lt: 75 } 
    }).toArray();

    let html = '<h2>Students with Attendance Below 75%</h2>';
    if (lowAttendanceStudents.length === 0) {
      html += '<p>No students found with attendance below 75%.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>Name</th><th>USN</th><th>Branch</th><th>Total Classes</th><th>Attended</th><th>Attendance %</th></tr>';
      lowAttendanceStudents.forEach(student => {
        const bgColor = student.attendancePercentage < 50 ? '#ffcccc' : '#ffe6cc';
        html += `<tr>
          <td>${student.name}</td>
          <td>${student.usn}</td>
          <td>${student.branch}</td>
          <td>${student.totalClasses}</td>
          <td>${student.attendedClasses}</td>
          <td style="background-color: ${bgColor}; font-weight: bold;">${student.attendancePercentage}%</td>
        </tr>`;
      });
      html += '</table>';
      html += `<p><strong>Total students with low attendance: ${lowAttendanceStudents.length}</strong></p>`;
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
    const db = client.db('attendancedb');
    const collection = db.collection('students');

    const allStudents = await collection.find({}).toArray();

    let html = '<h2>All Students Attendance</h2>';
    if (allStudents.length === 0) {
      html += '<p>No students found.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>Name</th><th>USN</th><th>Branch</th><th>Total Classes</th><th>Attended</th><th>Attendance %</th></tr>';
      allStudents.forEach(student => {
        const bgColor = student.attendancePercentage >= 75 ? '#ccffcc' : 
                       student.attendancePercentage >= 50 ? '#ffe6cc' : '#ffcccc';
        html += `<tr>
          <td>${student.name}</td>
          <td>${student.usn}</td>
          <td>${student.branch}</td>
          <td>${student.totalClasses}</td>
          <td>${student.attendedClasses}</td>
          <td style="background-color: ${bgColor}; font-weight: bold;">${student.attendancePercentage}%</td>
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

app.listen(5011, () => {
  console.log('Q11(b) - Listening on port 5011');
}); 