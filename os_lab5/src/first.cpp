extern "C" float E (int x);
extern "C" float Square (float A, float B);

float E (int x) {
    if (x < 0) {
        return -1;
    }
    float e = 1.0;
    for (int i = 0; i < x; ++i) {
        e *= 1 + 1/(float)x;
    }
    return e;
}

float Square (float A, float B) {
    if (A <= 0 || B <= 0) {
        return -1;
    }
    float square = A * B;
    return square;
}