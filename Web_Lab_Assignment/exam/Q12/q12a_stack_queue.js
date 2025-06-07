class Stack {
    constructor() {
        this.items = [];
    }
    
    push(element) {
        this.items.push(element);
    }
    
    pop() {
        return this.items.pop();
    }
    
    peek() {
        return this.items[this.items.length - 1];
    }
}

class Queue {
    constructor() {
        this.items = [];
    }
    
    enqueue(element) {
        this.items.push(element);
    }
    
    dequeue() {
        return this.items.shift();
    }
    
    front() {
        return this.items[0];
    }
}

const stack = new Stack();
const queue = new Queue();

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