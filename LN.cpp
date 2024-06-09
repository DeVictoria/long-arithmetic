#include "LN.h"

LN::LN(long long value)
{
	long long value2 = value;
	int len = 0;
	while (value2 != 0)
	{
		value2 /= 16;
		len++;
	}
	if (len == 0)
		len = 1;
	m_size = (len + 7) / 8;
	m_number = do_realloc(m_number, m_size);
	if (value < 0)
	{
		m_sign = '-';
		value = -value;
	}
	else
		m_sign = '+';
	for (int i = 0; i < m_size; ++i)
	{
		m_number[i] = value % 16;
		value /= 16;
		for (int j = 1; j < 8; ++j)
		{
			m_number[i] += (long long)pow(16, j) * (value % 16);
			value /= 16;
		}
	}
}

LN::LN(const LN &copyLN) :
	m_size{ copyLN.m_size }, m_number{ do_malloc(copyLN.m_size) }, m_sign{ copyLN.m_sign }, m_NaN{ copyLN.m_NaN }
{
	for (unsigned long long i = 0; i < copyLN.m_size; i++)
		m_number[i] = copyLN.m_number[i];
}

LN::LN(LN &&moveLN) noexcept :
	m_size{ moveLN.m_size }, m_number{ moveLN.m_number }, m_sign{ moveLN.m_sign }, m_NaN{ moveLN.m_NaN }
{
	moveLN.m_number = do_malloc(1);
	moveLN.m_size = 1;
	moveLN.m_sign = '+';
	moveLN.m_NaN = false;
}

LN::LN(std::string_view str)
{
	char *new_str = (char *)malloc(sizeof(char) * (str.length() + 1));
	if (new_str == nullptr)
	{
		throw "Not enough memory";
	}
	for (unsigned long long i = 0; i < str.length(); i++)
	{
		new_str[i] = str[i];
	}
	new_str[str.length()] = '\0';
	get_number(new_str);
}

LN &LN::operator=(const LN &copyLN)
{
	free(m_number);
	m_number = do_malloc(copyLN.m_size);
	m_size = copyLN.m_size;
	m_sign = copyLN.m_sign;
	m_NaN = copyLN.m_NaN;
	for (unsigned long long i = 0; i < copyLN.m_size; i++)
	{
		m_number[i] = copyLN.m_number[i];
	}
	return *this;
}

LN &LN::operator=(LN &&moveLN) noexcept
{
	free(m_number);
	m_number = moveLN.m_number;
	m_size = moveLN.m_size;
	m_sign = moveLN.m_sign;
	m_NaN = moveLN.m_NaN;
	moveLN.m_number = do_malloc(1);
	moveLN.m_size = 1;
	moveLN.m_sign = '+';
	moveLN.m_NaN = false;
	return *this;
}

LN LN::operator++()
{
	*this += LN(1);
	return *this;
}
LN LN::operator--()
{
	*this -= LN(1);
	return *this;
}

LN LN::operator++(int)
{
	*this += LN(1);
	return *this - LN(1);
}
LN LN::operator--(int)
{
	*this -= LN(1);
	return *this + LN(1);
}

LN LN::operator+()
{
	uint32_t *newLn = do_malloc(m_size);
	for (int i = 0; i < m_size; i++)
		newLn[i] = m_number[i];
	return { newLn, m_size, m_sign, m_NaN };
}
LN LN::operator-()
{
	uint32_t *newLn = do_malloc(m_size);
	for (int i = 0; i < m_size; i++)
		newLn[i] = m_number[i];
	return { newLn, m_size, m_sign == '+' ? '-' : '+', m_NaN };
}

LN LN::operator+(const LN &val)
{
	if (m_sign == val.m_sign)
	{
		if (m_sign == '-')
			return sum(val, '-');
		else
			return sum(val, '+');
	}
	else
	{
		if (m_sign == '-')
		{
			m_sign = '+';
			if (more(*this, val))
				return subtract(*this, val, '+');
			else
				return subtract(val, *this, '-');
		}
		else
		{
			m_sign = '-';
			if (more(*this, val))
				return subtract(val, *this, '-');
			else
				return subtract(*this, val, '+');
		}
	}
}
LN LN::operator-(const LN &val)
{
	if (m_sign == val.m_sign)
	{
		if (m_sign == '-')
		{
			if (more(*this, val))
				return subtract(val, *this, '+');

			else
				return subtract(*this, val, '-');
		}
		else
		{
			if (more(*this, val))
				return subtract(*this, val, '+');
			else
				return subtract(val, *this, '-');
		}
	}
	else
	{
		if (m_sign == '-')
			return sum(val, '-');
		else
			return sum(val, '+');
	}
}
LN LN::operator*(const LN &val)
{
	if (this->m_NaN)
	{
		LN x{ "0" };
		x.m_NaN = true;
		return x;
	}
	if ((m_size == 1 & m_number[0] == 0) | (val.m_size == 1 & val.m_number[0] == 0))
	{
		uint32_t *newLn = do_malloc(1);
		newLn[0] = 0;
		return { newLn, 1, '+', (bool)(m_NaN | val.m_NaN) };
	}
	int new_size = m_size + val.m_size - 1;
	uint32_t *newLn = do_malloc(new_size + 1);
	for (unsigned long long i = 0; i < new_size + 1; i++)
	{
		newLn[i] = 0;
	}
	unsigned long long transition = 0;
	for (unsigned long long i = 0; i < m_size; i++)
	{
		newLn[i + val.m_size - 1] += transition;
		transition = 0;
		for (unsigned long long j = 0; j < val.m_size; j++)
		{
			unsigned long long value = (unsigned long long)m_number[i] * val.m_number[j] + transition + newLn[i + j];
			newLn[i + j] = value % 4294967296;
			transition = value / 4294967296;
		}
	}
	if (transition > 0)
	{
		new_size++;
		newLn[new_size - 1] = transition;
	}
	return { newLn, new_size, (m_sign == val.m_sign ? '+' : '-'), (bool)(m_NaN | val.m_NaN) };
}
LN LN::operator/(const LN &val)
{
	char newSign = (m_sign == val.m_sign) ? '+' : '-';
	m_sign = '+';
	LN val2 = val;
	val2.m_sign = '+';
	if ((val.m_size == 1 & val.m_number[0] == 0) || this->m_NaN)
	{
		LN x{ "0" };
		x.m_NaN = true;
		return x;
	}
	else if ((m_size == 1 & m_number[0] == 0) | (*this < val2))
		return LN{ "0" };
	int new_size = m_size - val.m_size + 2;
	LN left;
	LN right;
	if (new_size > 2)
	{
		uint32_t *LnL = do_malloc(new_size - 2);
		uint32_t *LnR = do_malloc(new_size);
		for (unsigned long long i = 0; i < new_size - 3; i++)
		{
			LnL[i] = 0;
			LnR[i] = 0;
		}
		LnL[new_size - 3] = 1;
		LnR[new_size - 3] = 0;
		LnR[new_size - 2] = 0;
		LnR[new_size - 1] = 1;
		left = LN{ LnL, new_size - 2, '+', false };
		right = LN{ LnR, new_size, '+', false };
	}
	else
	{
		left = LN{ "0" };
		right = LN{ 4294967296 } * LN{ 4294967296 };
	}
	LN val_plus = val;
	val_plus.m_sign = '+';
	while ((left + LN{ 1 }) < right)
	{
		LN m = (left + right) * LN{ 2147483648 };
		for (unsigned long long i = 1; i < m.m_size; i++)
		{
			m.m_number[i - 1] = m.m_number[i];
		}
		m.m_size--;
		if (m.m_size == 0)
		{
			m.m_size++;
			m.m_number[0] = 0;
		}
		if ((m * val_plus) <= *this)
			left = m;
		else
			right = m;
	}
	if ((right * val_plus) <= *this)
	{
		right.m_sign = newSign;
		right.m_number = do_realloc(right.m_number, right.m_size);
		return right;
	}
	else
	{
		left.m_sign = newSign;
		left.m_number = do_realloc(left.m_number, left.m_size);
		return left;
	}
}
LN LN::operator%(const LN &val)
{
	if (this->m_NaN)
	{
		LN x{ "0" };
		x.m_NaN = true;
		return x;
	}
	LN val2 = *this / val;
	return *this - (val2 * val);
}
LN LN::operator~()
{
	if ((*this < LN{ "0" }) || this->m_NaN)
	{
		LN x{ "0" };
		x.m_NaN = true;
		return x;
	}
	else if (m_size == 1 & m_number[0] == 0)
		return LN{ "0" };
	int new_size = m_size / 2 + 2;
	LN left;
	LN right;
	if (new_size > 2)
	{
		uint32_t *LnL = do_malloc(new_size - 2);
		uint32_t *LnR = do_malloc(new_size);
		for (unsigned long long i = 0; i < new_size - 3; i++)
		{
			LnL[i] = 0;
			LnR[i] = 0;
		}
		LnL[new_size - 3] = 1;
		LnR[new_size - 3] = 0;
		LnR[new_size - 2] = 0;
		LnR[new_size - 1] = 1;
		left = LN{ LnL, new_size - 2, '+', false };
		right = LN{ LnR, new_size, '+', false };
	}
	else
	{
		left = LN{ "0" };
		right = LN{ 4294967296 } * LN{ 4294967296 };
	}
	while ((left + LN{ 1 }) < right)
	{
		LN m = (left + right) * LN{ 2147483648 };
		for (unsigned long long i = 1; i < m.m_size; i++)
		{
			m.m_number[i - 1] = m.m_number[i];
		}
		m.m_size--;
		if (m.m_size == 0)
		{
			m.m_size++;
			m.m_number[0] = 0;
		}
		if ((m * m) <= *this)
			left = m;
		else
			right = m;
	}
	if ((right * right) <= *this)
	{
		right.m_number = do_realloc(right.m_number, right.m_size);
		return right;
	}
	else
	{
		left.m_number = do_realloc(left.m_number, left.m_size);
		return left;
	}
}

LN::operator bool() const noexcept
{
	if (m_NaN | (m_size == 1 & m_number[0] == 0))
		return false;
	return true;
}
LN::operator long long() const noexcept
{
	if (m_NaN)
		return NAN;
	long long ans = 0;
	if (m_size <= 2)
	{
		for (int i = 0; i < m_size; i++)
			ans += m_number[i] * (long long)pow(4294967296, i);
	}
	else
	{
		return 0;
	}
	if (m_sign == '-')
		return -ans;
	return ans;
}

char *LN::print()
{
	if (m_NaN)
	{
		char *ans = (char *)malloc(sizeof(char) * 4);
		ans[0] = 'N';
		ans[1] = 'a';
		ans[2] = 'N';
		ans[3] = '\0';
		return ans;
	}
	if (m_size == 1 && m_number[0] == 0)
	{
		char *ans = (char *)malloc(sizeof(char) * 2);
		ans[0] = '0';
		ans[1] = '\0';
		return ans;
	}
	int char_size = (m_size - 1) * 8 + 1 + (m_sign == '-') + (int)log2(m_number[m_size - 1]) / 4 + 1;
	char *ans = (char *)malloc(sizeof(char) * char_size);
	if (ans == nullptr)
	{
		throw "Not enough memory";
	}
	if (m_sign == '-')
		ans[0] = '-';
	for (int i = 0; i < char_size - 1 - (m_sign == '-'); i++)
	{
		ans[char_size - i - 1 - 1] =
			get_char((int)((m_number[i / 8] % (long long)pow(16, i % 8 + 1)) / (long long)pow(16, i % 8)));
	}
	ans[char_size - 1] = '\0';
	return ans;
}

LN::LN(uint32_t *number1, int size1, char sign1, bool NaN1) :
	m_size{ size1 }, m_number{ number1 }, m_sign{ sign1 }, m_NaN{ NaN1 }
{
}

uint32_t LN::get_numb(const char &numb)
{
	if ((int)numb >= 48 & (int)numb <= 57)
		return (int)numb - 48;
	else if ((int)toupper(numb) >= 65 & (int)toupper(numb) <= 70)
		return (int)toupper(numb) - 55;
	else
		m_NaN = true;
	return 0;
}

int LN::equals(const LN &val) const
{
	if (m_size != val.m_size)
		return 0;
	if (m_NaN | val.m_NaN)
		return 0;
	if ((m_size == 1) & (m_number[0] == 0) & (val.m_number[0] == 0))
		return 1;
	if (m_sign != val.m_sign)
		return 0;
	for (unsigned long long i = 0; i < m_size; i++)
	{
		if (m_number[i] != val.m_number[i])
			return 0;
	}
	return 1;
}

int LN::more(const LN &val1, const LN &val2)
{
	if ((val1.m_size == 1) & (val2.m_size == 1) & (val1.m_number[0] == 0) & (val2.m_number[0] == 0))
		return 0;
	if (val1.m_NaN | val2.m_NaN)
		return 0;
	if (val1.m_sign != val2.m_sign)
	{
		return val1.m_sign == '+' ? 1 : 0;
	}
	if ((val1.m_sign == '+') & (val1.m_size != val2.m_size))
		return val1.m_size > val2.m_size ? 1 : 0;
	if ((val1.m_sign == '-') & (val1.m_size != val2.m_size))
		return val1.m_size < val2.m_size ? 1 : 0;
	for (unsigned long long i = val1.m_size; i > 0; i--)
	{
		if (val1.m_number[i - 1] < val2.m_number[i - 1])
			return val1.m_sign == '+' ? 0 : 1;
		if (val1.m_number[i - 1] > val2.m_number[i - 1])
			return val1.m_sign == '+' ? 1 : 0;
	}
	return 0;
}
LN LN::subtract(const LN &val1, const LN &val2, char new_sign)
{
	int new_size = val1.m_size;
	uint32_t *newLn = do_malloc(new_size);
	long long transition = 0;
	for (unsigned long long i = 0; i < val1.m_size; i++)
	{
		long long value = (long long)val1.m_number[i] - (val2.m_size > i ? (long long)val2.m_number[i] : 0) - transition;
		newLn[i] = value >= 0 ? value : 4294967296 + value;
		transition = value < 0 ? 1 : 0;
	}
	int count = 0;
	while (newLn[val1.m_size - count - 1] == 0)
		count++;
	if (count > 0)
	{
		new_size -= count;
		newLn = do_realloc(newLn, new_size);
	}
	return { newLn, new_size, new_sign, (bool)(val1.m_NaN | val2.m_NaN) };
}

LN LN::sum(const LN &val, char new_sign) const
{
	int new_size = std::max(m_size, val.m_size);
	uint32_t *newLn = do_malloc(new_size);
	unsigned long long transition = 0;
	for (unsigned long long i = 0; i < new_size; i++)
	{
		unsigned long long value =
			(m_size > i ? (unsigned long long)m_number[i] : 0) + (val.m_size > i ? (unsigned long long)val.m_number[i] : 0) + transition;
		newLn[i] = value % 4294967296;
		transition = value > 4294967295 ? 1 : 0;
	}
	if (transition == 1)
	{
		new_size++;
		newLn = do_realloc(newLn, new_size);
		newLn[new_size - 1] = 1;
	}
	return { newLn, new_size, new_sign, (bool)(m_NaN | val.m_NaN) };
}
char LN::get_char(int numb)
{
	if (numb >= 0 & (int)numb <= 9)
		return (char)(numb + 48);
	return (char)(numb + 55);
}
void LN::get_number(const char *str)
{
	int len = 0;
	while (str[len] != '\0')
		len++;
	if ((len == 0) | (len == 1 && (str[0] == '-' || str[0] == '0')) | (len == 2 & str[0] == '0' && str[1] == 'x') |
		(len == 3 & str[0] == '-' & str[1] == '0' & str[2] == 'x'))
	{
		m_size = 1;
		m_sign = '+';
		m_number[0] = 0;
	}
	else
	{
		int start;
		m_sign = str[0] == '-' ? '-' : '+';
		start = str[0] == '-' ? 1 : 0;
		if (len > (start + 2) & str[start] == '0' & str[start + 1] == 'x')
			start += 2;
		while (str[start] == '0')
			start++;
		if (start == len)
			start = len - 1;
		m_size = (len - start + 7) / 8;
		m_number = do_realloc(m_number, m_size);
		for (int i = 0; i < m_size; ++i)
		{
			m_number[i] = get_numb(str[len - i * 8 - 1]);
			for (int j = 1; j < 8; ++j)
			{
				m_number[i] += (long long)pow(16, j) * ((start + i * 8 + j) < len ? get_numb(str[len - i * 8 - j - 1]) : 0);
			}
		}
	}
}

uint32_t *LN::do_realloc(uint32_t *number1, int new_size)
{
	uint32_t *number2 = (uint32_t *)malloc(sizeof(uint32_t) * new_size);
	if (!number1)
	{
		free(number1);
		throw "Not enough memory";
	}
	for (unsigned long long i = 0; i < new_size; i++)
	{
		number2[i] = number1[i];
	}
	free(number1);
	return number2;
}
uint32_t *LN::do_malloc(int new_size)
{
	uint32_t *number1 = (uint32_t *)malloc(sizeof(uint32_t) * new_size);
	if (number1 == nullptr)
	{
		free(number1);
		throw "Not enough memory";
	}
	return number1;
}

LN operator"" _ln(const char *m)
{
	return LN(m);
}