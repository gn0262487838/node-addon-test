const addon = require('./build/Release/cshisX-addon');

// Initialize COM
const isInitialized = addon.initializeCOM();
if (!isInitialized) {
    console.error("Failed to initialize COM object.");
    process.exit(1);
}

try {
    // Call the VPNGetRandomX function
    const randomHex = addon.VPNGetRandomX();
    console.log("Random HEX String:", randomHex);

    // Call the VPNH_SignX function with a sample input string
    const signedStr = addon.VPNH_SignX(randomHex, '3', '30');
    console.log("Signed String:", signedStr);

} catch (error) {
    console.error("Error calling COM methods:", error);
}

// Clean up COM resources
addon.cleanupCOM();
