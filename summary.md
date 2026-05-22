# Summary cheatsheet

### 1.1. Fermet's theorem
---
Fermet's primality test revoles around one general theorem that states $a^{p-1} \equiv 1 \pmod{p}$ for $p$ being prime and $a$ being coprime. That theorem is rooted in **Euler's totient function**. By using this theorem we can pick $2\le a \le p -2$ and check it's logical value.

Example implementation of Fermat's primality test

```cpp
bool probablyPrimeFermat(int n, int iter=5) {
    if (n < 4)
        return n == 2 || n == 3;

    for (int i = 0; i < iter; i++) {
        int a = 2 + rand() % (n - 3);
        if (binpower(a, n - 1, n) != 1)
            return false;
    }
    return true;
}
```


### 1.2. Miller-Rabin's primality test
---
Miller-Rabin is an extension of Fermet's primality test. The general idea of it is that for an odd number $n$, $n-1$ is even and we factor out all powers of 2.

$$n-1=2^s \cdot d$$

This then gives us a $d$ variable that is always odd. We can use this fact and apply it to earlier mentioned Fermet's theorem which gives us 




