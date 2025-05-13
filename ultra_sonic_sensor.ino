const int trigPin = 5;
const int echoPin = 18;

// Speed of sound in cm/us
#define SOUND_SPEED 0.034

long duration;
float distanceCm;

void setup() {
  Serial.begin(115200);           // Start serial communication
  pinMode(trigPin, OUTPUT);       // Trigger pin as output
  pinMode(echoPin, INPUT);        // Echo pin as input
}

void loop() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Trigger the sensor: HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo duration
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  distanceCm = duration * SOUND_SPEED / 2;

  // Show only if the object is within a reasonable hand range
  if (distanceCm > 2 && distanceCm < 100) {
    Serial.print("Hand Distance: ");
    Serial.print(distanceCm);
    Serial.println(" cm");
  } else {
    Serial.println("Out of range or no object detected");
  }

  delay(300); // Refresh rate (adjust as needed)
}
