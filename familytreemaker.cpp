#include <bits/stdc++.h>
using namespace std;

#define DBG(a) cout << "line " << __LINE__ << " : " << #a << " --> " << (a) << endl
#define NL cout << endl
template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &p)
{
	os << "{" << p.first << "," << p.second << "}";
	return os;
}

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
string strip(string s)
{
	while (!s.empty() and isspace(s.back()))
		s.pop_back();
	reverse(s.begin(), s.end());

	while (!s.empty() and isspace(s.back()))
		s.pop_back();

	reverse(s.begin(), s.end());
	return s;
}

string remove_spaces(string s)
{
	string ret;
	for (char c : s)
		if (!isspace(c))
			ret += c;
	return ret;
}

class Person;
class Relation;
map<string, Person *> persons, husband;
map<string, Relation *> relations; /// stores relation of a father
class Person
{
public:
	map<string, string> attributes;
	Person(string line)
	{
		stringstream sin(line);
		getline(sin, attributes["name"], '(');
		attributes["name"] = strip(attributes["name"]);
		vector<string> parameters;
		string parameter;
		while (getline(sin, parameter, ','))
		{
			parameters.push_back(strip(parameter));
		}

		if (!parameters.empty())
		{
			parameters.back().pop_back();
		}
		// DBG(&parameters);
		// for(auto i: parameters)	cout<<i<<" ^ ";
		// NL;
		for (string parameter : parameters)
		{
			parameter = strip(parameter);
			if (parameter.empty())
				continue;
			if (parameter.size() == 1)
			{
				attributes["gender"] = parameter;
			}
			else
			{
				if (parameter.find('=') != string::npos)
				{
					int equal_sign = parameter.find('=');
					assert(parameter[equal_sign] == '=');
					string key = strip(parameter.substr(0, equal_sign));
					string value = strip(parameter.substr(equal_sign + 1));
					transform(key.begin(), key.end(), key.begin(), ::tolower);
					attributes[key] = value;
				}
			}
		}
		if (attributes.find("id") == attributes.end())
		{
			attributes["id"] = remove_spaces(attributes["name"]);
			while (persons.find(attributes["id"]) != persons.end())
			{
				attributes["id"] = attributes["id"] + char('0' + rng() % 10);
			}
		}
		if (attributes.find("gender") == attributes.end())
		{
			throw "Gender Missing For person " + attributes["name"];
		}
	}

	string &operator[](const string &key)
	{
		return attributes[key];
	}

	string label_record()
	{
		// string ret = "";
		// ret += "\"";
		// ret += "<" + attributes["id"] + ">";
		// ret += attributes["name"];
		// ret += "\"";
		// return ret;

		return "\"" + sublabel_record() + "\"";
	}
	string sublabel_record()
	{
		string ret = "";
		ret += "<" + attributes["id"] + ">";
		ret += attributes["name"];
		for (auto information : attributes)
		{
			if (information.first == "id" or information.first == "name")
				continue;
			ret += "\\n" + information.first + " : " + information.second;
		}
		return ret;
	}

	string bgcolor()
	{
		return attributes["gender"] == "M" ? "aquamarine" : "chartreuse";
	}

	string sublabel_table(int colspan = 1)
	{
		string ret = "<td align=\"text\" port=\"" + attributes["id"] + "\" bgcolor=\"" + bgcolor() + "\" colspan=\"" + to_string(colspan) + "\">";
		ret += attributes["name"];

		for (auto information : attributes)
		{
			if (information.first == "id" or information.first == "name" or information.first == "gender")
				continue;
			ret += "<br/>" + information.first + " : " + information.second;
		}

		ret += "</td>";
		return ret;
	}

	string label_table()
	{
		string ret = "<<table border=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n";
		ret += "<tr>\n\t\t" + sublabel_table() + "\n</tr>\n</table>>";
		return ret;
	}

	friend ostream &operator<<(ostream &os, const Person &p)
	{
		os << "[";
		for (auto i : p.attributes)
			os << i << " ";
		os << "]";
		return os;
	}
};

class Relation
{
public:
	Person *father;
	vector<pair<Person *, vector<Person *>>> mother_sons;
	Relation(Person *father, Person *mother) : father(father)
	{
		mother_sons.emplace_back(mother, vector<Person *>());
	}
	string label_record()
	{
		string ret = "\"{" + father->sublabel_record() + "|{";
		assert(!mother_sons.empty());
		ret += mother_sons[0].first->sublabel_record();
		for (int i = 1; i < mother_sons.size(); i++)
			ret += "|" + mother_sons[i].first->sublabel_record();
		ret += "}}\"";
		return ret;
	}
	string label_table()
	{
		string ret = "<<table border=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n";
		ret += "<tr>\n\t\t" + father->sublabel_table(mother_sons.size()) + "\n</tr>\n";
		ret += "<tr>\n";
		for (int i = 0; i < mother_sons.size(); i++)
		{
			ret += "\t\t"+mother_sons[i].first->sublabel_table() + "\n";
		}
		ret += "</tr>\n</table>>";
		return ret;
	}
	friend ostream &operator<<(ostream &os, const Relation &r)
	{
		assert(r.father);
		os << "relation [ " << r.father->attributes["id"];
		for (int i = 0; i < r.mother_sons.size(); i++)
		{
			assert(r.mother_sons[i].first);
			os << " { " << r.mother_sons[i].first->attributes["id"] << " ( ";
			for (int j = 0; j < r.mother_sons[i].second.size(); j++)
			{
				assert(r.mother_sons[i].second[j]);
				os << r.mother_sons[i].second[j]->attributes["id"] << " , ";
			}
			os << " ) } ";
		}
		os << "  ]  ";
		return os;
	}
};

void parse(ifstream &fin)
{
	string line;

	Relation *r = 0;
	int idx = -1;
	Person *last = 0;
	while (getline(fin, line))
	{
		//DBG(line);
		if (line[0] == '#')
			continue;
		//DBG(line);
		if (strip(line).empty())
		{
			r = 0;
			idx = 0;
			last = 0;
			continue;
		}
		Person *p = new Person(strip(line));
		if (persons[p->attributes["id"]] == 0)
			persons[(*p)["id"]] = p;
		p = persons[p->attributes["id"]];
		// DBG(*(p));

		if (line[0] == '\t')
		{
			assert(r);
			assert(idx != -1);
			assert(idx < r->mother_sons.size());
			r->mother_sons[idx].second.push_back(p);
			last = 0;
		}
		else
		{
			r = 0;
			idx = -1;
			if (last)
			{
				Person *father = last;
				Person *mother = p;
				if ((*mother)["gender"] == "M")
					swap(father, mother);
				assert((*father)["gender"] == "M");
				assert((*mother)["gender"] == "F");
				husband[mother->attributes["id"]] = father;
				if (relations.find(father->attributes["id"]) != relations.end())
				{
					r = relations[father->attributes["id"]];
					for (int i = 0; i < r->mother_sons.size(); i++)
					{
						if (r->mother_sons[i].first == mother)
						{
							idx = i;
							break;
						}
					}
					if (idx == -1)
					{
						idx = r->mother_sons.size();
						r->mother_sons.emplace_back(mother, vector<Person *>());
					}
				}
				else
				{
					r = new Relation(father, mother);
					relations[father->attributes["id"]] = r;
					idx = 0;
				}
			}
			else
				last = p;
		}
	}
}

string get_node_port_of_child(Person *p)
{
	Relation *r = 0;
	if (p->attributes["gender"] == "M")
		r = relations[p->attributes["id"]];
	else
	{
		if (husband.find(p->attributes["id"]) != husband.end())
			r = relations[husband[p->attributes["id"]]->attributes["id"]];
	}
	if (r)
		return r->father->attributes["id"] + ":" + p->attributes["id"];
	else
		return p->attributes["id"] + ":" + p->attributes["id"];
}

string get_node_using_record(Person *p, Relation *r)
{
	if (r)
		return "\t" + r->father->attributes["id"] + "[label = " + r->label_record() + " , shape=record];";
	else
		return "\t" + p->attributes["id"] + "[label = " + p->label_record() + " , shape=record];";
}

string get_node_using_table(Person *p, Relation *r)
{
	if (r)
		return "\t" + r->father->attributes["id"] + "[label = " + r->label_table() + " , shape = plaintext ];\n";
	else
		return "\t" + p->attributes["id"] + "[label = " + p->label_table() + " , shape = plaintext ];\n";
}

void graphviz(string ancestor)
{

	cout << "digraph {\n\tgraph[bgcolor=bisque2 , ranksep=\"2\"];\n\tedge[dir=none];\n";

	vector<string> gen;
	gen.push_back(ancestor);
	while (!gen.empty())
	{
		vector<string> gen2;
		// DBG(&gen);
		vector<string> edges;

		cout << "{\n\trank=same;\n";

		for (string person_id : gen)
		{
			// cout << (person_id) << "  ,,  ";
			Person *p = persons[person_id];
			Relation *r = 0;
			if (p->attributes["gender"] == "M")
			{
				r = relations[person_id];
			}
			else
			{
				if (husband.find(person_id) != husband.end())
					r = relations[husband[person_id]->attributes["id"]];
			}
			// DBG(r);

			cout << get_node_using_table(p, r) << endl;

			if (r)
				for (auto i : r->mother_sons)
				{
					for (auto j : i.second)
					{
						edges.push_back((r ? r->father->attributes["id"] : person_id) + ":" + i.first->attributes["id"] + "->" + get_node_port_of_child(j));
						gen2.push_back(j->attributes["id"]);
					}
				}
		}

		cout << "}\n";

		for (string edge : edges)
			cout << "\t\t" + edge << ";\n";
		// NL;
		swap(gen, gen2);
	}
	cout << "}" << endl;
}

int main(int argc, char **argv)
{
	assert(1 <= argc);
	vector<string> args(argc - 1);
	for (int i = 0; i < argc - 1; i++)
		args[i] = string(argv[i + 1]);
	if (args.size() < 2)
	{
		cout << "pass name of input text file , ancestor id in parameter" << endl;
	}
	else
	{
		// for (auto arg : args)
		// 	DBG(arg);
		// NL;

		string ancestor = args[1];

		ifstream fin(args[0]);
		if (fin.is_open())
		{
			// DBG("File Opened Successfully");

			parse(fin);
			// DBG("Parsing Completed");
			if (persons.find(ancestor) == persons.end())
			{
				cout << "Ancestor not found" << endl;
				return 0;
			}

			graphviz(ancestor);
			// for(auto i: persons )
			// 	cout<<i<<"\n";
			// NL;
		}
		else
		{
			cout << "Unable to open file" << endl;
		}
	}
}