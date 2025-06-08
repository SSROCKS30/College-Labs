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

  if (!name || !usn || !subject || isNaN(parsedMarks)) {
    return res.send('All fields are required and marks must be a valid number');
  }

  if (parsedMarks < 0 || parsedMarks > 100) {
    return res.send('Marks must be between 0 and 100');
  }

  let client;
  try {
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('exammarksdb');
    const collection = db.collection('student_marks');

    await collection.insertOne({ name, usn, subject, marks: parsedMarks });

    res.send(`Student ${name} marks (${parsedMarks}) for ${subject} added successfully! <br><a href="/">Go Back</a>`);

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
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('exammarksdb');
    const collection = db.collection('student_marks');

    const lowMarksStudents = await collection.find({ 
      marks: { $lt: 20 } 
    }).toArray();

    let html = '<h2>Students with Marks Less Than 20</h2>';
    if (lowMarksStudents.length === 0) {
      html += '<p>No students found with marks less than 20.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>Name</th><th>USN</th><th>Subject</th><th>Marks</th><th>Status</th></tr>';
      lowMarksStudents.forEach(student => {
        const status = student.marks < 10 ? 'Critical' : 'Needs Improvement';
        const bgColor = student.marks < 10 ? '#ffcccc' : '#ffe6cc';
        html += `<tr>
          <td>${student.name}</td>
          <td>${student.usn}</td>
          <td>${student.subject}</td>
          <td style="background-color: ${bgColor}; font-weight: bold;">${student.marks}</td>
          <td style="background-color: ${bgColor}; font-weight: bold;">${status}</td>
        </tr>`;
      });
      html += '</table>';
      html += `<p><strong>Total students with marks < 20: ${lowMarksStudents.length}</strong></p>`;
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
    client = await MongoClient.connect(uri, { useUnifiedTopology: true });
    const db = client.db('exammarksdb');
    const collection = db.collection('student_marks');

    const allStudents = await collection.find({}).toArray();

    let html = '<h2>All Student Marks</h2>';
    if (allStudents.length === 0) {
      html += '<p>No student marks found.</p>';
    } else {
      html += '<table border="1" style="border-collapse: collapse;">';
      html += '<tr><th>Name</th><th>USN</th><th>Subject</th><th>Marks</th><th>Grade</th></tr>';
      allStudents.forEach(student => {
        let grade, bgColor;
        if (student.marks >= 90) { grade = 'A+'; bgColor = '#90EE90'; }
        else if (student.marks >= 80) { grade = 'A'; bgColor = '#ADD8E6'; }
        else if (student.marks >= 70) { grade = 'B'; bgColor = '#FFE4B5'; }
        else if (student.marks >= 60) { grade = 'C'; bgColor = '#DDA0DD'; }
        else if (student.marks >= 50) { grade = 'D'; bgColor = '#F0E68C'; }
        else if (student.marks >= 35) { grade = 'E'; bgColor = '#FFA07A'; }
        else { grade = 'F'; bgColor = '#ffcccc'; }
        
        html += `<tr>
          <td>${student.name}</td>
          <td>${student.usn}</td>
          <td>${student.subject}</td>
          <td style="background-color: ${bgColor}; font-weight: bold;">${student.marks}</td>
          <td style="background-color: ${bgColor}; font-weight: bold;">${grade}</td>
        </tr>`;
      });
      html += '</table>';
      html += `<p><strong>Total student records: ${allStudents.length}</strong></p>`;
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