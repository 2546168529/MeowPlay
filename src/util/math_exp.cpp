#include "math_exp.h"
#include <stack>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <math.h>

/**
** ��������������ȡ����ջ��Ԫ��
** @return ��stack���Ԫ�ز����������򷵻�false��ʾ��ȡʧ��*/
template <typename T>
inline bool rpn_top2(std::stack<T> &rpn, T &e, T &s)
{
	if (rpn.size() < 2)
		return false;
	e = rpn.top();
	rpn.pop();
	s = rpn.top();
	rpn.pop();
	return true;
}

/**
** �����沨�����ʽ
** @param _rpn_exp �沨�����ʽ��
** @param _exp_len �沨�����ʽ���ĳ���
** @return �������ռ����� */
double calculate_rpn(const char *_rpn_exp, size_t _exp_len)
{
	constexpr size_t N_BUF_MAXSIZE = 32;

	std::stack<double> rpn;

	char number_buf[N_BUF_MAXSIZE];
	size_t buf_pos = 0;

	for (size_t p = 0; p < _exp_len; ++p)
	{
		if (_rpn_exp[p] == ' ')
		{
			//�����ո������ֻ�������������������һ��ȡ��ɣ�ת����ֱ�Ӽ���
			if (buf_pos != 0)
			{
				rpn.push(atof(number_buf));
				buf_pos = 0;
			}
		}
		else if (_rpn_exp[p] == '.' || (_rpn_exp[p] >= '0' && _rpn_exp[p] <= '9'))
		{
			//�ж�������뵽�������Ƿ񳬳���Χ
			if (buf_pos + 1 >= N_BUF_MAXSIZE)
				return 0;
			number_buf[buf_pos++] = _rpn_exp[p];
			number_buf[buf_pos] = '\0';
		}
		else
		{
			//e��stack��һ�ε�����ֵ��s��stack�ڶ��ε�����ֵ
			double s, e;
			switch (_rpn_exp[p])
			{
			case '+':
				if (!rpn_top2(rpn, e, s))
					return 0;
				rpn.push(s + e);
				break;

			case '-':
				if (!rpn_top2(rpn, e, s))
					return 0;
				rpn.push(s - e);
				break;

			case '*':
				if (!rpn_top2(rpn, e, s))
					return 0;
				rpn.push(s * e);
				break;

			case '/':
				if (!rpn_top2(rpn, e, s) || e == 0)
					return 0;
				rpn.push(s / e);
				break;

			case '%':
				if (!rpn_top2(rpn, e, s) || e == 0)
					return 0;
				rpn.push(static_cast<int64_t>(s) % static_cast<int64_t>(e));
				break;

			case '^':
				if (!rpn_top2(rpn, e, s))
					return 0;
				rpn.push(pow(s, e));
				break;
			}
		}
	}

	//���������Ϻ�stackʣ��ĳ�Ա����Ϊ1��ʧ��
	if (rpn.size() != 1)
		return 0;

	return rpn.top();
}

/**
** ��һ����ѧ��ʽ����Ϊ�沨�����ʽ
** @param _math_exp ���ʽ��
** @param _exp_len ���ʽ���ĳ���
** @return �����沨�����ʽ */
std::string make_rpn(const char *_math_exp, size_t _exp_len)
{
	bool first = true;

	std::stack<char> rpn_op;
	std::string result;

	for (size_t p = 0; p < _exp_len; ++p)
	{
		if (_math_exp[p] == ' ')
			continue;

		//��ͷ��һ����Ч����
		if (first)
		{
			//�����ͷ��һ����Ч������+����-�����ڿ�ͷ��һ��0
			if (_math_exp[p] == '-' || _math_exp[p] == '+')
			{
				result.append(1, '0');
				result.append(1, ' ');
			}

			first = false;
		}

		//���ڱ�ʶ�Ƿ�׷�ӹ����֣���׷�ӹ����֣�����Ҫ����ɺ���ĩβ����ո�
		bool flag = false;
		while ((_math_exp[p] >= '0' && _math_exp[p] <= '9') || _math_exp[p] == '.' || _math_exp[p] == ' ')
		{
			if (_math_exp[p] != ' ')
			{
				result.append(1, _math_exp[p]);
				flag = true;
			}

			if (++p >= _exp_len)
				break;
		}

		if (flag)
			result.append(1, ' ');
		if (p >= _exp_len)
			break;

		//��ʱ���������ڼ�¼��ջ����
		char top_ch;

		if (_math_exp[p] == ')')
		{
			while (!rpn_op.empty())
			{
				top_ch = rpn_op.top();
				rpn_op.pop();
				if (top_ch == '(')
					break;
				result.append(1, top_ch);
				result.append(1, ' ');
			}
		}
		else if (_math_exp[p] == '+' || _math_exp[p] == '-')
		{
			//StackΪ�գ�ֱ��push
			if (rpn_op.empty())
			{
				rpn_op.push(_math_exp[p]);
			}
			else
			{
				while (!rpn_op.empty())
				{
					top_ch = rpn_op.top();
					//�����ҵ����ȼ��ȼӼ���С�ģ�����ȫ����ջ
					//�����������ȼ�
					if (top_ch == '(')
						break;

					result.append(1, top_ch);
					result.append(1, ' ');
					rpn_op.pop();
				}

				rpn_op.push(_math_exp[p]);
			}
		}
		else if (_math_exp[p] == '(')
		{
			//�����ţ�������ֱ�Ӽ���
			rpn_op.push(_math_exp[p]);
		}
		else if (_math_exp[p] == '*' || _math_exp[p] == '/' || _math_exp[p] == '%' || _math_exp[p] == '^')
		{
			while (!rpn_op.empty())
			{
				top_ch = rpn_op.top();
				//�����ҵ����ȼ��ȼ�������Ĳ�����С�ģ�����ȫ����ջ
				if (top_ch == '(' || top_ch == '+' || top_ch == '-')
					break;

				result.append(1, top_ch);
				result.append(1, ' ');
				rpn_op.pop();
			}

			rpn_op.push(_math_exp[p]);
		}
	}

	while (!rpn_op.empty())
	{
		result.append(1, rpn_op.top());
		result.append(1, ' ');
		rpn_op.pop();
	}

	return result;
}

double calculate_expr(const char *_expr)
{
	std::string rpn_exp = make_rpn(_expr, strlen(_expr));
	return calculate_rpn(rpn_exp.c_str(), rpn_exp.length());
}
