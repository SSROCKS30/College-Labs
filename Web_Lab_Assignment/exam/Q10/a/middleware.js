// Visit counter variable
let visitCount = 0;

// Custom middleware function to log requests
const requestLogger = (req, res, next) => {
    const timestamp = new Date().toISOString();
    const method = req.method;
    const url = req.url;
    const ip = req.ip || req.connection.remoteAddress;
    
    console.log(`[${timestamp}] ${method} ${url} - IP: ${ip}`);
    next(); // Continue to next middleware/route
};

// Custom middleware function to count visits
const visitCounter = (req, res, next) => {
    visitCount++;
    console.log(`Total visits: ${visitCount}`);
    
    // Add visit count to response locals so it can be accessed in routes
    res.locals.visitCount = visitCount;
    next();
};

module.exports = {
    requestLogger,
    visitCounter
}; 