#!/usr/bin/python

path = "phone_contacts_20160830.backup"

def from_hex(text):
    name = []
    for i in range(0, len(text), 2):
        name.append(chr(int(text[i:i+2], 16)))
    return "".join(name)

class Record:

    def add(self, name, text):
        _, text = text.strip().split("=")
        text = text.strip()
        assert (text[0] == '"') and (text[-1] == '"'), text
        text = text[1:-1]
        #print name, text
        if name == "number":
            self.number = text
        elif name == "name":
            self.name = from_hex(text)
        else:
            raise Exception("not supported")

    def asdict(self):
        return {
            "FirstName" : self.name,
            "NumberGeneral" : self.number,
        }

    def __repr__(self):
        return "%s : %s" % (self.name, self.number)

def parse(path):
    records = []
    record = None

    for line in file(path):
        text = line.decode("utf-16-be").encode("utf8")
        if text.startswith("[Phone"):
            if record:
                #print record
                records.append(record)
            record = Record()
        elif text.startswith("Entry00Text"):
            record.add("number", text)
        elif text.startswith("Entry01Text"):
            record.add("name", text)

    #print record
    records.append(record)
    return records

#vc = """BEGIN:VCARD
#VERSION:3.0
#N:%(surname)s;%(forename)s
#FN:%(forename)s %(surname)s
#TEL;TYPE=MOBILE,VOICE:%(mobile)s
#TEL;TYPE=HOME,VOICE:%(phone)s
#EMAIL;TYPE=PREF,INTERNET:%(email)s
#REV:%(now)s
#END:VCARD"""

def show_info(info):
    if not info:
        return

    vcard = [ "BEGIN:VCARD", "VERSION:3.0" ]

    fields = (
        ("LastName", "N:%s"),
        ("FirstName", "FN:%s"),
        ("NumberGeneral", "TEL;TYPE=HOME,VOICE:%s"),
        ("NumberWork", "TEL;TYPE=WORK,VOICE:%s"),
        ("NumberMobile", "TEL;TYPE=MOBILE,VOICE:%s"),
        ("Email", "EMAIL;TYPE=PREF,INTERNET:%s"),
    )

    for field, fmt in fields:
        if field in info:
            vcard.append(fmt % info[field])

    vcard.append("REV:20130720T110254Z")
    vcard.append("END:VCARD")
 
    text = "\n".join(vcard)
    print text

#
#

records = parse(path)

for record in records:
    #print record
    show_info(record.asdict())

# FIN
