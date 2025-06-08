const express = require('express');
const app = express();
const { MongoClient } = require('mongodb');
const path = require('path');

const uri = 'mongodb://127.0.0.1:27017';

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '3b.html'));
});

app.get('/insert', async (req, res) => {
  const { emp_name, email, phone, hire_date, job_title, salary } = req.query;
  const parsedSalary = parseFloat(salary);

  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('hrdb');
    const collection = db.collection('employees');

    await collection.insertOne({ 
      emp_name: emp_name, 
      email: email, 
      phone: phone, 
      hire_date: new Date(hire_date), 
      job_title: job_title, 
      salary: parsedSalary 
    });

    res.send(`Employee ${emp_name} registered successfully! <br><a href="/">Go Back</a>`);

  } catch (err) {
    console.error('Error:', err);
    res.status(500).send('Internal server error');
  } finally {
    if (client) {
      await client.close();
    }
  }
});

app.get('/high-salary', async (req, res) => {
  let client;
  try {
    client = await MongoClient.connect(uri);
    const db = client.db('hrdb');
    const collection = db.collection('employees');

    const highSalaryEmployees = await collection.find({ salary: { $gt: 50000 } }).toArray();

    let html = '<h2>Employees with Salary > 50000</h2>';
    if (highSalaryEmployees.length === 0) {
      html += '<p>No employees found with salary > 50000.</p>';
    } else {
      html += '<ul>';
      highSalaryEmployees.forEach(emp => {
        html += `<li> Name: ${emp.emp_name} | Email: ${emp.email} | Phone: ${emp.phone} | Hire Date: ${emp.hire_date.toDateString()} | Job Title: ${emp.job_title} | Salary: ₹${emp.salary} </li>`;
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

app.listen(5003, () => {
  console.log('Q3(b) - Listening on port 5003');
}); 