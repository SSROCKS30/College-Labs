import { stack } from './stack.js';
import { queue } from './queue.js';

function stackPush() {
    const value = document.getElementById('stackInput').value;
    if (value) {
        stack.push(value);
        document.getElementById('stackInput').value = '';
        document.getElementById('stackDisplay').innerHTML = '[' + stack.items.join(', ') + ']';
        document.getElementById('stackResult').innerHTML = 'Pushed: ' + value;
    }
}

function stackPop() {
    const result = stack.pop();
    document.getElementById('stackDisplay').innerHTML = '[' + stack.items.join(', ') + ']';
    document.getElementById('stackResult').innerHTML = 'Popped: ' + result;
}

function stackPeek() {
    const result = stack.peek();
    document.getElementById('stackResult').innerHTML = 'Top: ' + result;
}

function queueEnqueue() {
    const value = document.getElementById('queueInput').value;
    if (value) {
        queue.enqueue(value);
        document.getElementById('queueInput').value = '';
        document.getElementById('queueDisplay').innerHTML = '[' + queue.items.join(', ') + ']';
        document.getElementById('queueResult').innerHTML = 'Enqueued: ' + value;
    }
}

function queueDequeue() {
    const result = queue.dequeue();
    document.getElementById('queueDisplay').innerHTML = '[' + queue.items.join(', ') + ']';
    document.getElementById('queueResult').innerHTML = 'Dequeued: ' + result;
}

function queueFront() {
    const result = queue.front();
    document.getElementById('queueResult').innerHTML = 'Front: ' + result;
}

// Make functions globally accessible for onclick handlers
window.stackPush = stackPush;
window.stackPop = stackPop;
window.stackPeek = stackPeek;
window.queueEnqueue = queueEnqueue;
window.queueDequeue = queueDequeue;
window.queueFront = queueFront; 