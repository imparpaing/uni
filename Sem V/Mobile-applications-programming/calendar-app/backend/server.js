'use strict';

import express from 'express';
import cors from 'cors';
import { authenticateAndGetClient } from './auth.js';
import {
  listEvents,
  addEvent,
  listTasklists,
  listTasks,
  addTask,
} from './taskManager.js';

const app = express();
const port = 3001;

// Enable CORS for all routes
app.use(cors());

// Parse JSON
app.use(express.json());

// Parse URL-encoded bodies
app.use(express.urlencoded({ extended: true }));

app.get('/api/v1/calendar', async (req, res) => {
  try {
    // Perform calendar related operations
    const [calendar, service] = await authenticateAndGetClient();
    const events = (await listEvents(calendar)) || [];
    const tasklists = (await listTasklists(service)) || [];
    const tasks = (await listTasks(service)) || [];

    // Send the response back to the frontend
    res.json({ events, tasklists, tasks });
  } catch (err) {
    console.error('Error:', err);
    res.status(500).json({ error: 'Internal server error' });
  }
});

app.post('/api/v1/calendar/new-entry', async (req, res) => {
  try {
    const { title, description, dateTime, type } = req.body;
    const [calendar, service] = await authenticateAndGetClient();

    if (type === 'event') {
      const event = {
        summary: title,
        description: description,
        start: {
          dateTime: dateTime,
          timeZone: 'CET',
        },
        end: {
          dateTime: dateTime,
          timeZone: 'CET',
        },
      };

      await addEvent(calendar, event);
    } else {
      const task = {
        title: title,
        notes: description,
        due: dateTime,
      };

      await addTask(service, task);
    }

    res.status(200).json({ message: 'Event added successfully!' });
  } catch (err) {
    console.error('Error', err);
    res.status(500).json({ error: 'Internal server error' });
  }
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});
