#ifndef LN_H
#define LN_H

#include <string_view>

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>

class LN
{
  public:
	// default Constructor:
	explicit LN(long long value = 0);

	// copy
	LN(const LN &);

	// move
	LN(LN &&) noexcept;

	// Constructors:
	explicit LN(const char *str) { get_number(str); }
	explicit LN(std::string_view);

	// Destructor:
	~LN() { free(m_number); }

	// bool Operators:
	int operator==(const LN &val) const noexcept { return equals(val); }
	int operator!=(const LN &val) const noexcept { return 1 - equals(val); }
	int operator>=(const LN &val) const noexcept { return (equals(val) | more(*this, val)); }
	int operator>(const LN &val) const noexcept { return more(*this, val); }
	int operator<=(const LN &val) const noexcept { return (equals(val) | more(val, *this)); }
	int operator<(const LN &val) const noexcept { return more(val, *this); }

	// copyOperator:
	LN &operator=(const LN &);

	// moveOperator:
	LN &operator=(LN &&) noexcept;

	// assignment operators:
	LN &operator+=(const LN &val) { return *this = *this + val; };
	LN &operator-=(const LN &val) { return *this = *this - val; };
	LN &operator*=(const LN &val) { return *this = *this * val; };
	LN &operator/=(const LN &val) { return *this = *this / val; };
	LN &operator%=(const LN &val) { return *this = *this % val; };

	// prefix
	LN operator++();
	LN operator--();

	// postfix
	LN operator++(int);
	LN operator--(int);

	// arithmetic operations
	LN operator+();
	LN operator-();
	LN operator+(const LN &);
	LN operator-(const LN &);
	LN operator*(const LN &);
	LN operator/(const LN &);
	LN operator%(const LN &);
	LN operator~();

	// Operators type conversions
	explicit operator bool() const noexcept;
	explicit operator long long() const noexcept;

	// print
	char *print();

  private:
	int m_size = 1;
	uint32_t *m_number = do_malloc(1);
	char m_sign = '+';
	bool m_NaN = false;

	// help constructor
	LN(uint32_t *, int, char, bool);

	// help function
	uint32_t get_numb(const char &);
	int equals(const LN &) const;
	static int more(const LN &, const LN &);
	static LN subtract(const LN &, const LN &, char);
	LN sum(const LN &, char) const;
	static char get_char(int);
	void get_number(const char *);
	static uint32_t *do_realloc(uint32_t *, int);
	static uint32_t *do_malloc(int);
};
LN operator"" _ln(const char *);
#endif	  // LN_H