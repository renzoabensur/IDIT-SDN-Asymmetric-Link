import math,random
num_bits =32
max_num = 2**num_bits
current_estimate = 1
print max_num

while (math.log(current_estimate + 1, 2)/num_bits < 0.0000015):
  current_estimate <<= 1
  current_estimate += 1
  print math.log(current_estimate + 1, 2)
  #print current_estimate

print "{0:032b}".format(current_estimate)

j =0
acumulated_fails = 0
success_rate = 0.8
for i in range(128):
  if random.random() > success_rate:
    acumulated_fails += 1
  else:
    if acumulated_fails > 0:
      #acumulated_fails += 1
      current_estimate >>= (acumulated_fails)
    else:
      current_estimate <<= 1
      current_estimate += 1
    current_estimate %= max_num
    print i, "{0:032b}".format(current_estimate), acumulated_fails, math.log(current_estimate + 1, 2)/ num_bits
    acumulated_fails = 0

current_estimate = 0
for i in range(128):
  if random.random() > success_rate:
    acumulated_fails += 1
  else:
    current_estimate <<= (acumulated_fails + 1)
    current_estimate += 1
    current_estimate %= max_num
    print i, "{0:032b}".format(current_estimate), acumulated_fails, 1.0 * "{0:032b}".format(current_estimate).count('1')/ num_bits
    acumulated_fails = 0

success_rate = 0.2
for i in range(128):
  if random.random() > success_rate:
    acumulated_fails += 1
  else:
    current_estimate <<= (acumulated_fails + 1)
    current_estimate += 1
    current_estimate %= max_num
    print i, "{0:032b}".format(current_estimate), acumulated_fails, 1.0 * "{0:032b}".format(current_estimate).count('1')/ num_bits
    acumulated_fails = 0

success_rates = range(1, 11)
success_rates = map(lambda x: x*1.0/10, success_rates)
random.shuffle(success_rates)
print success_rates

last_informed_metric = 1.0 * "{0:032b}".format(current_estimate).count('1')/ num_bits
last_estimative = 1.0 * "{0:032b}".format(current_estimate).count('1')/ num_bits
for success_rate in success_rates:
  print success_rate
  max_v = 0
  min_v = 1
  for i in range(128):
    if random.random() > success_rate:
      acumulated_fails += 1
    else:
      current_estimate <<= (acumulated_fails + 1)
      current_estimate += 1
      current_estimate %= max_num
      percentage =  1.0 * "{0:032b}".format(current_estimate).count('1')/ num_bits
      last_estimative = last_estimative * 0.1 + percentage * 0.9
    #   last_estimative = last_estimative * 0 + percentage * 1
      if i > 32 or True:
        max_v = max(percentage, max_v)
        min_v = min(percentage, min_v)
      acumulated_fails = 0
      if abs(last_estimative - last_informed_metric) > 0.2:
        print i, "informed! ",  "%.2f -> %.2f" % (last_informed_metric,last_estimative)
        last_informed_metric = last_estimative
  print success_rate, "%.2f %.2f" % (min_v, max_v)
  print

print "############### exp 1 ######################\n"

success_rates = (0.2, 0.4, 0.7, 0.9)

threshold = {}
threshold[8] = 12.5/100
threshold[16] = 12.5/100
threshold[32] = 9.375/100

min_error_dict, max_error_dict, average_error_dict = {}, {}, {}
average_error_to_i_dict, inf_count_dict = {}, {}

n_samples = 128000

for success_rate in success_rates:
  print success_rate,
  last_informed_metric = -1
  sum_abs_error = 0.0
  sum_abs_error_to_informed = 0.0
  n_estimates = 0
  estimative_pct = 0.0
  max_error = 0
  min_error = 1
  max_error_fullbuffer = 0
  count_failures = 0
  count_successes = 0
  history = 0
  inf_c = 0
  received_messages = 0
  for i in range(n_samples):
    if random.random() > success_rate:
      acumulated_fails += 1
    else:
      received_messages += 1
      if acumulated_fails >= num_bits:
        history = (1 << (num_bits - 1))
        count_failures = num_bits - 1;
        count_successes = 1
      elif count_failures + count_successes + acumulated_fails + 1 > num_bits:
        count_successes += 1
        count_failures += acumulated_fails
        while acumulated_fails + 1:
          count_failures -= ((~ history) & 0x1);
          count_successes -= (history & 0x1);
          acumulated_fails -= 1
          history >>= 1
        history |= (1 << (num_bits - 1))
      else:
        history |= (1 << (count_failures + count_successes + acumulated_fails))
        count_successes += 1
        count_failures += acumulated_fails
      acumulated_fails = 0

      estimative_pct = 1.0 * count_successes / (count_failures + count_successes)
      sum_abs_error += abs(success_rate - estimative_pct)
      n_estimates += 1
      if count_failures + count_successes > num_bits - 1:
        max_error_fullbuffer = max(max_error_fullbuffer, abs(success_rate - estimative_pct))
      max_error = max(max_error, abs(success_rate - estimative_pct))
      min_error = min(min_error, abs(success_rate - estimative_pct))

      if abs(estimative_pct - last_informed_metric) > threshold[num_bits] or last_informed_metric == -1:
        last_informed_metric = estimative_pct
        inf_c += 1
      sum_abs_error_to_informed += abs(success_rate - last_informed_metric)
  print ("%.3f %.3f %.3f %.3f") % (min_error, sum_abs_error/n_estimates, max_error_fullbuffer, 1.0*inf_c/received_messages)
  min_error_dict[success_rate] = min_error
  max_error_dict[success_rate] = max_error_fullbuffer
  average_error_dict[success_rate] = sum_abs_error/n_estimates
  average_error_to_i_dict[success_rate] = sum_abs_error_to_informed/n_estimates
  inf_count_dict[success_rate] = 1.0*inf_c/received_messages

keyes = sorted(average_error_to_i_dict.keys())
print
for k in keyes:
    print k, '\t',
print
print 'min_error'
for k in keyes:
    print min_error_dict[k], '\t',
print
print 'max_error'
for k in keyes:
    print max_error_dict[k], '\t',
print
print 'avg_error'
for k in keyes:
    print average_error_dict[k], '\t',
print
print 'avg_error_to_informed'
for k in keyes:
    print average_error_to_i_dict[k], '\t',
print
print 'informed_count %'
for k in keyes:
    print inf_count_dict[k], '\t',
print

print "\n############### exp 2 ######################\n"

transitions = [ (0.9, 0.8), (0.3, 0.2), (0.9, 0.2), (0.6, 0.4) ]

for i in range(len(transitions)):
  transitions.insert(i*2+1, (transitions[i*2][1],transitions[i*2][0]))
# transitions.extend(map(lambda (i,j): (j,i), transitions))


threshold = {}
threshold[8] = 12.5/100
threshold[16] = 12.5/100
threshold[32] = 9.375/100

min_error_dict, max_error_dict, average_error_dict = {}, {}, {}
average_error_to_i_dict, inf_count_dict = {}, {}

n_samples = 50000
for (success_rate1, success_rate2) in transitions:
  print success_rate1, "->", success_rate2, "\t",
print
for (success_rate1, success_rate2) in transitions:
  # print success_rate1, "->", success_rate2
  sum_number_of_estimates_to_converge = 0
  for sample in range(n_samples):
    success_rate = success_rate1
    last_informed_metric = -1
    sum_abs_error = 0.0
    sum_abs_error_to_informed = 0.0
    n_estimates = 0
    estimative_pct = 0.0
    max_error = 0
    min_error = 1
    max_error_fullbuffer = 0
    count_failures = 0
    count_successes = 0
    history = 0
    inf_c = 0
    received_messages = 0
    rounds = 0
    # for i in range(n_samples):
    while True:
      rounds += 1
      if random.random() > success_rate:
        acumulated_fails += 1
      else:
        received_messages += 1
        if acumulated_fails >= num_bits:
          history = (1 << (num_bits - 1))
          count_failures = num_bits - 1;
          count_successes = 1
        elif count_failures + count_successes + acumulated_fails + 1 > num_bits:
          count_successes += 1
          count_failures += acumulated_fails
          while acumulated_fails + 1:
            count_failures -= ((~ history) & 0x1);
            count_successes -= (history & 0x1);
            acumulated_fails -= 1
            history >>= 1
          history |= (1 << (num_bits - 1))
        else:
          history |= (1 << (count_failures + count_successes + acumulated_fails))
          count_successes += 1
          count_failures += acumulated_fails
        acumulated_fails = 0

        estimative_pct = 1.0 * count_successes / (count_failures + count_successes)
        sum_abs_error += abs(success_rate - estimative_pct)
        n_estimates += 1
        if count_failures + count_successes > num_bits - 1:
          max_error_fullbuffer = max(max_error_fullbuffer, abs(success_rate - estimative_pct))
        max_error = max(max_error, abs(success_rate - estimative_pct))
        min_error = min(min_error, abs(success_rate - estimative_pct))

        if abs(estimative_pct - last_informed_metric) > threshold[num_bits] or last_informed_metric == -1:
          last_informed_metric = estimative_pct
          inf_c += 1
          if success_rate == success_rate2:
            pass
            sum_number_of_estimates_to_converge += rounds
            break

        if success_rate == success_rate2 and abs(success_rate2 - last_informed_metric) <= 0.125:
          # print n_estimates - num_bits
          pass
          # sum_number_of_estimates_to_converge += rounds
          # break
        sum_abs_error_to_informed += abs(success_rate - last_informed_metric)

        if success_rate == success_rate1 and count_failures + count_successes == num_bits:
          success_rate = success_rate2
          rounds = 0
          # print n_estimates, " renan"
    # print ("%.3f %.3f %.3f %.3f") % (min_error, sum_abs_error/n_estimates, max_error_fullbuffer, 1.0*inf_c/received_messages)
    min_error_dict[success_rate] = min_error
    max_error_dict[success_rate] = max_error_fullbuffer
    average_error_dict[success_rate] = sum_abs_error/n_estimates
    average_error_to_i_dict[success_rate] = sum_abs_error_to_informed/n_estimates
    inf_count_dict[success_rate] = 1.0*inf_c/received_messages
  print "%8.1f" % (1.0 * sum_number_of_estimates_to_converge/n_samples,), "\t",


print
print

for i in range(17):
    print  0xFF * i / 16

print 1<<3, 1<<1, ((1<<3) + (1<<1))/10



# print "=============================="
# LINK_ESTIMATE_UNIT = 8
# ALPHA = 3
# ETX_MIN_DELTA = 4
# ETX_MIN_RELATIVE = 30
#
# current_estimate = 1.5 * LINK_ESTIMATE_UNIT
# for success_rate in success_rates:
#   print success_rate
#
#   for i in range(128):
#     if random.random() > success_rate:
#       acumulated_fails += 1
#     else:
#       current_estimate = ((acumulated_fails * LINK_ESTIMATE_UNIT * ALPHA) + current_estimate * (LINK_ESTIMATE_UNIT - ALPHA))/LINK_ESTIMATE_UNIT
#       if ( (abs(current_estimate - last_informed_metric) > ETX_MIN_DELTA) and (current_estimate > last_informed_metric * (1.0 + ETX_MIN_RELATIVE / 100.0) or current_estimate < last_informed_metric / (1.0 + ETX_MIN_RELATIVE / 100.0))):
#         print i, "informed! ",  "%.2f -> %.2f" % (last_informed_metric, current_estimate)
#         last_informed_metric = current_estimate
#       acumulated_fails = 0
#   print
