// Step 1. Find the equation for the object you are
// interested in..  (e.g., cylinder)
//
// (x-a)^2 + (y - b)^2 + (z - c)^2 = r^2
//
// Step 2. Parameterize the equation with a center point
// if needed
//
// (x-Cx)^2 + (y-Cy)^2 + (z-Cz)^2 = r^2
//
// Step 3. Substitute the eq for a ray into our object
// equation.
//
// (Rox + t*Rdx - Cx)^2 + (Roy + t*Rdy - Cy)^2 + (Roz + t*Rdz - Cz)^2 - r^2 = 0
//
// Step 4. Solve for t.
//
// Step 4a. Rewrite the equation (flatten).
//
// -r^2 +
// t^2 * Rdx^2 +
// t^2 * Rdy^2 +
// t^2 * Rdz^2 +

// 2*t * Rox * Rdx -
// 2*t * Rdx * Cx +

// 2*t * Roy * Cx -
// 2*t * Rdy * Cx +

// 2*t * Roz * Rdz -
// 2*t * Rdz * Cz +

// Rox^2 -
// 2*Rox*Cx +
// Cx^2 +

// Roy^2 -
// 2*Roy*Cy +
// Cy^2 +

// Roz^2 -
// 2*Roz*Cz +
// Cz^2 = 0
//
// Steb 4b. Rewrite the equation in terms of t.
//
// t^2 * (Rdx^2 + Rdy^2 + Rdz^2) +
// t * (2 * (Rox * Rdx - Rdx * Cx + Roy * Rdy - Rdy * Cy + Roz * Rdz - Rdz * Cz)) +
// Rox^2 - 2*Rox*Cx + Cx^2 + Roy^2 - 2*Roy*Cy + Cy^2 +
// Roz^2 - 2*Roz*Cz + Cz^2 - r^2 = 0
//
// Use the quadratic equation to solve for t..

double a = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
double b = (2 * (Ro[0] * Rd[0] - Rd[0] * C[0] + Ro[1] * Rd[1] - Rd[1] * C[1] + Ro[2] * Rd[2] - Rd[2] * C[2]));
double c = sqr(Ro[0]) - 2*Ro[0]*C[0] + sqr(C[1]) + Ro[1]) - 2*Ro[1]*C[1] + sqr(C[1]) + sqr(Ro[2]) - 2*Ro[2]*C[2] + sqr(C[2]) - sqr(r);
