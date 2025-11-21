const request = require('supertest');
const { expect } = require('chai');

describe('End-to-End Integration Tests', () => {
    const API_URL = process.env.API_URL || 'http://localhost:5000';

    describe('Complete Attendance Flow', () => {
        it('should complete full attendance marking flow', async () => {
            // 1. Student login
            const loginRes = await request(API_URL)
                .post('/api/auth/login')
                .send({
                    email: 'student@test.com',
                    password: 'password123'
                });

            expect(loginRes.status).to.equal(200);
            const token = loginRes.body.token;

            // 2. Mark attendance
            const attendanceRes = await request(API_URL)
                .post('/api/attendance/mark')
                .set('Authorization', `Bearer ${token}`)
                .send({
                    studentId: loginRes.body.user.id,
                    classId: 'CLASS_001',
                    nfcTagId: 'TEST123'
                });

            expect(attendanceRes.status).to.equal(201);

            // 3. Verify attendance was recorded
            const checkRes = await request(API_URL)
                .get(`/api/attendance/student/${loginRes.body.user.id}`)
                .set('Authorization', `Bearer ${token}`);

            expect(checkRes.status).to.equal(200);
            expect(checkRes.body.attendance.length).to.be.greaterThan(0);
        });
    });

    describe('Emergency Alert Flow', () => {
        it('should trigger and process emergency alert', async () => {
            // Login
            const loginRes = await request(API_URL)
                .post('/api/auth/login')
                .send({
                    email: 'student@test.com',
                    password: 'password123'
                });

            const token = loginRes.body.token;

            // Trigger SOS
            const sosRes = await request(API_URL)
                .post('/api/emergency/alert')
                .set('Authorization', `Bearer ${token}`)
                .send({
                    studentId: loginRes.body.user.id,
                    alertType: 'SOS',
                    location: 'Test Location'
                });

            expect(sosRes.status).to.equal(201);
            expect(sosRes.body.alert).to.have.property('status', 'active');
        });
    });
});
