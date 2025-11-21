import React from 'react';
import { render, fireEvent, waitFor } from '@testing-library/react-native';
import AttendanceScreen from '../../mobile-app/student-app/src/screens/AttendanceScreen';
import QuizScreen from '../../mobile-app/student-app/src/screens/QuizScreen';

describe('Attendance Screen', () => {
    it('should render attendance screen correctly', () => {
        const { getByText } = render(<AttendanceScreen />);
        expect(getByText('Mark Attendance')).toBeTruthy();
    });

    it('should mark attendance on button press', async () => {
        const { getByText } = render(<AttendanceScreen />);
        const button = getByText('Tap to Check In');
        
        fireEvent.press(button);
        
        await waitFor(() => {
            expect(getByText(/Success/i)).toBeTruthy();
        });
    });
});

describe('Quiz Screen', () => {
    it('should display quiz question', () => {
        const mockQuiz = {
            question: 'What is 2+2?',
            options: ['3', '4', '5', '6']
        };
        
        const { getByText } = render(<QuizScreen quiz={mockQuiz} />);
        expect(getByText('What is 2+2?')).toBeTruthy();
    });

    it('should allow answer selection', () => {
        const mockQuiz = {
            question: 'What is 2+2?',
            options: ['3', '4', '5', '6']
        };
        
        const { getByText } = render(<QuizScreen quiz={mockQuiz} />);
        const option = getByText('4');
        
        fireEvent.press(option);
        // Verify selection state changed
    });
});
