print ('hello')
def juldate(day, month, year):
    if year < 1:
        year = 1
    if month > 2:
        month -= 3
    else:
        month += 9
        year -= 1
    century = year / 100
    cy = year - 100 * century
    return (146097 * century) / 4 + (1461 * cy) / 4 + (153 * month + 2) / 5 + day + 1721119

print (juldate(7, 6, 1949))
print (juldate(8, 3, 2023))
